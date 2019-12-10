package helmsley.vr.DUIs;

import android.content.Context;
import android.content.res.Resources;
import android.content.res.TypedArray;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.SeekBar;
import android.widget.TextView;

import java.lang.ref.WeakReference;
import java.util.ArrayList;
import java.util.LinkedHashMap;

import helmsley.vr.JNIInterface;
import helmsley.vr.R;
import helmsley.vr.UIsManager;

public class SeekbarAdapter {
    private ArrayList<seekbarListAdapter> mAdapters = new ArrayList<>();
    private final WeakReference<Context> contexRef;
    private final WeakReference<UIsManager> mUIManagerRef;
    private final static String[] TuneTitles = {"Opacity", "Tunes"};
    private ArrayList<float[]>tune_maxs = new ArrayList<>();
    private ArrayList<int[]>tune_seek_max= new ArrayList<>();
    private ArrayList<LinkedHashMap<String, Float>> tune_maps= new ArrayList<>();

    public SeekbarAdapter(Context context, UIsManager manager){
        contexRef = new WeakReference<>(context);
        mUIManagerRef = new WeakReference<>(manager);
        //setup initial values
        Resources res = contexRef.get().getResources();
        //setup tune values
        setupTuneMapValue(res, R.array.texParams, 0 );
        setupTuneMapValue(res, R.array.raycastParams, 1);

        for(int i=0; i<TuneTitles.length; i++){
            LinkedHashMap vmap = tune_maps.get(i);
            mAdapters.add(new seekbarListAdapter(
                    contexRef.get(),
                    new ArrayList<>(vmap.keySet()),
                    new ArrayList<>(vmap.values()),
                    tune_maxs.get(i),
                    tune_seek_max.get(i),
                    TuneTitles[i]));
        }
    }
    private void setupTuneMapValue(Resources res, int paramID, int tex_ray_id) {
        TypedArray params = res.obtainTypedArray(paramID);
        int item_numbers = params.length();
        LinkedHashMap<String, Float> value_map = new LinkedHashMap<>();
        float[] max_values = new float[item_numbers];
        int[] max_seeks = new int[item_numbers];
        String[] names = new String[item_numbers];
        float[] values= new float[item_numbers];

        for (int i = 0; i < item_numbers; i++) {
            int resId = params.getResourceId(i, -1);
            if (resId == -1) continue;

            String param[] = res.getStringArray(resId);
            names[i] = param[0];
            values[i] = Float.valueOf(param[1]);
            value_map.put(names[i], values[i]);
            max_values[i] = Float.valueOf(param[2]);
            max_seeks[i] = Integer.valueOf(param[3]);
        }
        tune_maxs.add(max_values); tune_seek_max.add(max_seeks);tune_maps.add(value_map);

        JUIInterface.JUIInitTuneParam(tex_ray_id, item_numbers, names, values);
    }
    public seekbarListAdapter getListAdapter(int index){
        if(index > TuneTitles.length) return null;
        return mAdapters.get(index);
    }

    public class seekbarListAdapter extends ListAdapter {
        private ArrayList<Float> item_values;
        private float[] item_value_max;
        private int[] item_seek_max;
        private int dropview_width;

        public seekbarListAdapter(Context context,
                                  ArrayList<String> item_names, ArrayList<Float> item_values,
                                  float[] item_value_max, int[]  item_seek_max,
                                  String title) {
            super(context, title);

            this.item_names = item_names;
            this.item_values = item_values;
            this.item_value_max = item_value_max;
            this.item_seek_max = item_seek_max;
            this.dropview_width = (int)(Resources.getSystem().getDisplayMetrics().widthPixels *Float.valueOf(context.getString(R.string.cf_drop_tune_w)) );
        }

        @Override
        public View getDropDownView(int position, View convertView, ViewGroup parent) {
            ViewHolder holder;

            if (convertView == null) {
                holder = new ViewHolder();
                convertView = mInflater.inflate(R.layout.spinner_tune_layout, null);
                holder.text_name = (TextView) convertView.findViewById(R.id.tuneName);
                holder.text_value = (TextView) convertView.findViewById(R.id.tuneValue);

                holder.seekbar = (SeekBar) convertView.findViewById(R.id.tuneSeekbar);
                holder.seekbar.setMax(item_seek_max[position]);
                holder.seekbar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
                    @Override
                    public void onProgressChanged(SeekBar seekBar, int i, boolean b) {
                        float value = (float)i / item_seek_max[position] * item_value_max[position];
                        if(value == item_values.get(position)) return;
                        item_values.set(position, value);
                        holder.text_value.setText(contexRef.get().getString(R.string.text_value, item_values.get(position)));
                        JUIInterface.JUIsetTuneParam(UIsManager.getTexRayIdx(), item_names.get(position), item_values.get(position));
                    }
                    @Override
                    public void onStartTrackingTouch(SeekBar seekBar) {}
                    @Override
                    public void onStopTrackingTouch(SeekBar seekBar) {}
                });
                holder.seekbar.setTag(position);
                holder.seekbar.setProgress((int)(item_values.get(position) / item_value_max[position] * item_seek_max[position] ));
                convertView.setMinimumWidth(dropview_width);


                convertView.setTag(R.layout.spinner_tune_layout, holder);
            } else {
                holder = (ViewHolder) convertView.getTag(R.layout.spinner_tune_layout);
            }

            holder.text_name.setText(item_names.get(position));
            float value = item_values.get(position);

            holder.text_value.setText(contexRef.get().getString(R.string.text_value, value));

            return convertView;
        }
        class ViewHolder {
            TextView text_name;
            TextView text_value;
            SeekBar seekbar;
        }
    }
}