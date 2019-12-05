package helmsley.vr.DUIs;


import android.content.Context;
import android.content.res.Resources;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.SeekBar;
import android.widget.TextView;

import java.lang.ref.WeakReference;
import java.util.ArrayList;

import helmsley.vr.R;

public class SeekbarAdapter {
    private WeakReference<listAdapter> mAdapterRef = null;

    public listAdapter getListAdapter(Context context, ArrayList<String> item_names, ArrayList<Float> item_values,float[] item_value_max, int[] item_seek_max, String title) {
        if(mAdapterRef==null){
            listAdapter adapter = new listAdapter(context, item_names, item_values,item_value_max, item_seek_max, title);
            mAdapterRef = new WeakReference<>(adapter);
        }
        return mAdapterRef.get();
    }

    public class listAdapter extends BaseAdapter {
        private LayoutInflater mInflater;
        private ArrayList<String> item_names;
        private ArrayList<Float> item_values;
        private float[] item_value_max;
        private int[] item_seek_max;

        private String title;
        private int dropview_width;
        private final WeakReference<Context> contexRef;
        public listAdapter(Context context, ArrayList<String> item_names, ArrayList<Float> item_values, float[] item_value_max, int[]  item_seek_max,String title) {
            //todo : set initial value
            contexRef = new WeakReference<>(context);
            mInflater = LayoutInflater.from(context);
            this.item_names = item_names;
            this.item_values = item_values;
            this.item_value_max = item_value_max;
            this.item_seek_max = item_seek_max;
            this.title = title;
            dropview_width = (int)(Resources.getSystem().getDisplayMetrics().widthPixels *Float.valueOf(context.getString(R.string.cf_drop_tune_w)) );
        }

        @Override
        public int getCount() {
            return item_names.size();
        }

        @Override
        public Object getItem(int position) {
            // TODO Auto-generated method stub
            return null;
        }

        @Override
        public long getItemId(int position) {
            // TODO Auto-generated method stub
            return 0;
        }

        @Override
        public View getView(int position, View convertView, ViewGroup parent) {
            ViewHolder2 holder;

            if (convertView == null) {
                holder = new ViewHolder2();
                convertView = mInflater.inflate(R.layout.baseadapter_layout, null);
                holder.text_title = (TextView) convertView.findViewById(R.id.tuneHeader);
                convertView.setTag(R.layout.baseadapter_layout, holder);
            } else {
                holder = (ViewHolder2) convertView.getTag(R.layout.baseadapter_layout);
            }
            holder.text_title.setText(title);
            return convertView;
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

    }

    static class ViewHolder {
        TextView text_name;
        TextView text_value;
        SeekBar seekbar;
    }

    static class ViewHolder2 {
        TextView text_title;
    }
}