#version 310 es

#extension GL_EXT_shader_io_blocks:require
#extension GL_EXT_geometry_shader:require

precision mediump float;

layout (local_size_x = 8, local_size_y = 8, local_size_z = 8) in;

// layout(binding = 0, rgba8) readonly uniform mediump image3D u_volume;
// layout(binding = 0, r8) readonly uniform mediump image3D u_volume;
layout(binding = 0, r8ui)readonly uniform mediump uimage3D u_volume;


// layout (rgba16f, binding = 0) readonly uniform image3D u_volume;

layout(std140, binding = 0) writeonly buffer u_buffer_vertices
{
    vec4 output_vertices[];
};

layout(std140, binding = 1) writeonly buffer u_buffer_normals
{
    vec4 output_normals[];
};

layout(std430, binding = 2) readonly buffer u_buffer_triangle_table 
{
    int triangle_table[];
};

layout(std430, binding = 3) readonly buffer u_buffer_configuration_table 
{
    int configuration_table[];
};

// The current cell index (xyz)
const ivec3 cell_index = ivec3(gl_GlobalInvocationID.xyz);

// The indices of the 8 neighbors that form the boundary of this cell
const ivec3 neighbors[8] = 
{
	cell_index,
	cell_index + ivec3(  0, 0, 1 ),
	cell_index + ivec3( -1, 0, 1 ),
	cell_index + ivec3( -1, 0, 0 ),

	cell_index + ivec3(  0, 1, 0 ),
	cell_index + ivec3(  0, 1, 1 ),
	cell_index + ivec3( -1, 1, 1 ),
	cell_index + ivec3( -1, 1, 0 )
};

struct Vertex 
{
	vec3 position;
	vec3 normal;
};
uint u_maskbits = uint(31);
uint u_organ_num = uint(7);
Vertex find_vertex(float isolevel, in ivec2 edge, float value_1, float value_2)
{
	// Grab the two vertices at either end of the edge between `index_1` and `index_2`
	vec3 p1 = neighbors[edge.x];
	vec3 p2 = neighbors[edge.y];

	// The normals are stored in the YZW / GBA channels of the volume texture
	vec3 n1 = vec3(.0);//imageLoad(u_volume, ivec3(p1)).gba;
	vec3 n2 = vec3(.0);//imageLoad(u_volume, ivec3(p2)).gba;

	const float eps = 0.00001;

	if (abs(isolevel - value_1) < eps) 
	{
		return Vertex(p1, n1);
	}
	if (abs(isolevel - value_2) < eps) 
	{
		return Vertex(p2, n2);
	}
	if (abs(value_1 - value_2) < eps) 
	{
		return Vertex(p1, n1);
	}

	const float mu = (isolevel - value_1) / (value_2 - value_1);

	vec3 p = p1 + mu * (p2 - p1);
	vec3 n = n1 + mu * (n2 - n1);

	return Vertex(p, normalize(n));
}

const ivec2 edge_table[12] = 
{
	{ 0, 1 },
	{ 1, 2 },
	{ 2, 3 },
	{ 3, 0 },
	{ 4, 5 },
	{ 5, 6 },
	{ 6, 7 },
	{ 7, 4 },
	{ 0, 4 },
	{ 1, 5 },
	{ 2, 6 },
	{ 3, 7 }
};
int getMaskBit(uint mask_value){
    //check body
    if(mask_value == uint(0)) return ((u_maskbits & uint(1)) == uint(1))? 0:-1;

    int CHECK_BIT = int(-1);
    //check if organ
    for(uint i=uint(0); i<u_organ_num; i++){
        if(((u_maskbits>> uint(i + uint(1))) & uint(1)) == uint(0)) continue;
        uint cbit = (mask_value>> i) & uint(1);
        if(cbit == uint(1)){
            CHECK_BIT = int(i) + 1;
            break;
        }
    }
    return CHECK_BIT;
}
void march(in ivec3 cell_index)
{
	ivec3 volume_size = imageSize(u_volume);
	vec3 inv_volume_size = 1.0 / vec3(volume_size);

	// Avoid sampling outside of the volume bounds
	if (cell_index.x == 0 || 
		cell_index.y == (volume_size.y - 1) || 
		cell_index.z == (volume_size.z - 1)) 
	{
		return;
	}

	ivec3 neighbors[8] = 
	{
		cell_index,
		cell_index + ivec3(  0, 0, 1 ),
		cell_index + ivec3( -1, 0, 1 ),
		cell_index + ivec3( -1, 0, 0 ),

		cell_index + ivec3(  0, 1, 0 ),
		cell_index + ivec3(  0, 1, 1 ),
		cell_index + ivec3( -1, 1, 1 ),
		cell_index + ivec3( -1, 1, 0 )
	};

	// Calculate which of the 256 configurations this cell is 
	float values[8];
	int configuration = 0;
	for (int i = 0; i < 8; ++i)
	{
        uint sc = uint(imageLoad(u_volume, neighbors[i]).r);
		if(sc == uint(4))values[i] = -1.0;// sc.a;
		else values[i] = 1.0;
		if(values[i] < .0) configuration |= 1 << i;
	}

	// Normally, we could `return` here, but we want to make sure to "clear"
	// the vertex buffer at unused indices, otherwise the drawing gets super
	// wonky...
	if (configuration_table[configuration] == 0) 
	{		
	}

	// Grab all of the (interpolated) vertices along each of the 12 edges of this cell
	Vertex vertex_list[12];
	if (configuration_table[configuration] != 0) 
	{
		for (int i = 0; i < 12; ++i)
		{
			if (int(configuration_table[configuration] & (1 << i)) != 0)
			{	
				ivec2 edge = edge_table[i];
				vertex_list[i] = find_vertex(0, edge, values[edge.x], values[edge.y]);
			}
		}
	}
	
	// Construct triangles based on this cell's configuration and the vertices calculated above
	const vec4 empty = vec4(0.0);
	const int cell_start_memory = (cell_index.x + 
								   cell_index.y * volume_size.x + 
								   cell_index.z * volume_size.x * volume_size.y) * 15; // 15 = 5 tris per cell, 3 coordinates (xyz) each

	const int triangle_start_memory = configuration * 16; // 16 = the size of each "row" in the triangle table
	const int max_triangles = 5;

	for (int i = 0; i < max_triangles; ++i)
	{
		if (triangle_table[triangle_start_memory + 3 * i] != -1)
		{
			Vertex vertex;
			vec3 position;
			vec3 normal;

			vertex = vertex_list[triangle_table[triangle_start_memory + (3 * i + 0)]];
			position = vertex.position * inv_volume_size;
			position = position * 2.0 - 1.0;
			output_vertices[cell_start_memory +  3 * i + 0] = vec4(position, 1.0);
			output_normals[cell_start_memory +  3 * i + 0] = vec4(vertex.normal, 1.0);

			vertex = vertex_list[triangle_table[triangle_start_memory + (3 * i + 1)]];
			position = vertex.position * inv_volume_size;
			position = position * 2.0 - 1.0;
			output_vertices[cell_start_memory +  3 * i + 1] = vec4(position, 1.0);
			output_normals[cell_start_memory +  3 * i + 1] = vec4(vertex.normal, 1.0);

			vertex = vertex_list[triangle_table[triangle_start_memory + (3 * i + 2)]];
			position = vertex.position * inv_volume_size;
			position = position * 2.0 - 1.0;
			output_vertices[cell_start_memory +  3 * i + 2] = vec4(position, 1.0);
			output_normals[cell_start_memory +  3 * i + 2] = vec4(vertex.normal, 1.0);
		}
		else
		{
			output_vertices[cell_start_memory + (3 * i + 0)] = empty;
			output_vertices[cell_start_memory + (3 * i + 1)] = empty;
			output_vertices[cell_start_memory + (3 * i + 2)] = empty;
		}
	} 

	
// #ifdef DEBUG
// 	for (int i = 0; i < 5; i++)
// 	{
// 		output_vertices[cell_start_memory + (3 * i + 0)] = vec4(vec3(cell_index), 1.0);
// 		output_vertices[cell_start_memory + (3 * i + 1)] = vec4(vec3(cell_index) + vec3(0.5, 0, 0), 1.0);
// 		output_vertices[cell_start_memory + (3 * i + 2)] = vec4(vec3(cell_index) + vec3(0, 0.5, 0), 1.0);
// 	}
// #endif
}

void main()
{
	// Resolution of the 3D texture (W, H, D) - integer values
    ivec3 dims = imageSize(u_volume);

    // The 3D coordinates of this compute shader thread
    ivec3 cell_index = ivec3(gl_GlobalInvocationID.xyz);
	// cell_index = cell_index * 2;
	// if(cell_index.x %2 != 0 || cell_index.y %2 != 0 ||cell_index.z %2 != 0)
	// 	return;
	// else
    // Perform marching cubes
    march(cell_index);
}