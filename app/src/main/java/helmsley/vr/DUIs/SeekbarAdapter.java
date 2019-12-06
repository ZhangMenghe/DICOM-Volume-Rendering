package helmsley.vr.DUIs;

import android.content.Context;
import android.content.res.Resources;
import android.content.res.TypedArray;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.SeekBar;
import android.widget.TextView;

import java.lang.ref.WeakReference;
import java.util.ArrayList;
import java.util.LinkedHashMap;

import helmsley.vr.R;

public class SeekbarAdapter {
    private ArrayList<WeakReference<seekbarListAdapter>> mAdapterRefs= new ArrayList<>();
    private final WeakReference<Context> contexRef;

    private final static String[] TuneTitles = {"Opacity", "Tunes"};
    private ArrayList<float[]>tune_maxs = new ArrayList<>();
    private ArrayList<int[]>tune_seek_max= new ArrayList<>();
    private ArrayList<LinkedHashMap<String, Float>> tune_maps= new ArrayList<>();

    public SeekbarAdapter(Context context){
        contexRef = new WeakReference<>(context);
        for(int i=0; i<TuneTitles.length; i++){
            mAdapterRefs.add(null);
        }

        //setup initial values
        Resources res = contexRef.get().getResources();
        //setup tune values
        setupTuneMapValue(res, R.array.texParams);
        setupTuneMapValue(res, R.array.raycastParams);
    }
    private void setupTuneMapValue(Resources res, int paramID) {
        TypedArray params = res.obtainTypedArray(paramID);
        int item_numbers = params.length();
        LinkedHashMap<String, Float> value_map = new LinkedHashMap<>();
        float[] max_values = new float[item_numbers];
        int[] max_seeks = new int[item_numbers];

        for (int i = 0; i < params.length(); i++) {
            int resId = params.getResourceId(i, -1);
            if (resId == -1) continue;
            String param[] = res.getStringArray(resId);
            value_map.put(param[0], Float.valueOf(param[1]));
            max_values[i] = Float.valueOf(param[2]);
            max_seeks[i] = Integer.valueOf(param[3]);
        }
        tune_maxs.add(max_values); tune_seek_max.add(max_seeks);tune_maps.add(value_map);
    }
    public seekbarListAdapter createListAdapter(int index){
        if(index > TuneTitles.length) return null;
        if(mAdapterRefs.get(index)!=null) return mAdapterRefs.get(index).get();
        LinkedHashMap vmap = tune_maps.get(index);
        seekbarListAdapter adapter = new seekbarListAdapter(
                contexRef.get(),
                new ArrayList<>(vmap.keySet()),
                new ArrayList<>(vmap.values()),
                tune_maxs.get(index),
                tune_seek_max.get(index),
                TuneTitles[index]);
        mAdapterRefs.set(index, new WeakReference<>(adapter));
        return adapter;
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

                convertView.setTag(R.layout.spinner_tune_layout, holder);
            } else {
                holder = (ViewHolder) convertView.getTag(R.layout.spinner_tune_layout);
            }
            holder.text_name.setText(item_names.get(position));
            float value = item_values.get(position);

            holder.text_value.setText(contexRef.get().getString(R.string.text_value, value));
            holder.seekbar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
                @Override
                public void onProgressChanged(SeekBar seekBar, int i, boolean b) {
                    item_values.set(position, (float)i / item_seek_max[position] * item_value_max[position]);
                    holder.text_value.setText(contexRef.get().getString(R.string.text_value, item_values.get(position)));
                }
                @Override
                public void onStartTrackingTouch(SeekBar seekBar) {}
                @Override
                public void onStopTrackingTouch(SeekBar seekBar) {}
            });
            holder.seekbar.setTag(position);
            holder.seekbar.setProgress((int)(value / item_value_max[position] * item_seek_max[position] ));
            convertView.setMinimumWidth(dropview_width);
            return convertView;
        }
        class ViewHolder {
            TextView text_name;
            TextView text_value;
            SeekBar seekbar;
        }
    }


}