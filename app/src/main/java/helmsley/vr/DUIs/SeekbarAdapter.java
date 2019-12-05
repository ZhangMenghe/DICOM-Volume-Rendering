package helmsley.vr.DUIs;


import android.content.Context;
import android.content.res.Resources;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.SeekBar;
import android.widget.TextView;

import java.util.ArrayList;

import helmsley.vr.R;

public class SeekbarAdapter {
    private SeekBarListener mListener;

    public interface SeekBarListener {
        public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser, int positionInList);

        public void onStartTrackingTouch(SeekBar seekBar, int positionInList);

        public void onStopTrackingTouch(SeekBar seekBar, int positionInList);
    }

    public listAdapter getListAdapter(Context context, ArrayList<String> item_names, ArrayList<Float> item_values, String title) {
        return new listAdapter(context, item_names, item_values, title);
    }

    public void setSeekBarListener(SeekBarListener listener) {
        mListener = listener;
    }

    public class listAdapter extends BaseAdapter {
        private LayoutInflater mInflater;
        private onSeekbarChange mSeekListener;
//        private ArrayList<String> itemsList;
        private ArrayList<String> item_names;
        private String title;
        private int dropview_width;

        public listAdapter(Context context, ArrayList<String> item_names, ArrayList<Float> item_values, String title) {
            //todo : set initial value
            mInflater = LayoutInflater.from(context);
            if (mSeekListener == null) {
                mSeekListener = new onSeekbarChange();
            }
            this.item_names = item_names;
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
                holder.text = (TextView) convertView.findViewById(R.id.tuneName);
                holder.seekbar = (SeekBar) convertView.findViewById(R.id.tuneSeekbar);
                convertView.setTag(R.layout.spinner_tune_layout, holder);
            } else {
                holder = (ViewHolder) convertView.getTag(R.layout.spinner_tune_layout);
            }
            holder.text.setText(item_names.get(position));
            holder.seekbar.setOnSeekBarChangeListener(mSeekListener);
            holder.seekbar.setTag(position);
            convertView.setMinimumWidth(dropview_width);
            return convertView;

        }

    }

    static class ViewHolder {
        TextView text;
        SeekBar seekbar;
    }

    static class ViewHolder2 {
        TextView text_title;
    }


    public class onSeekbarChange implements SeekBar.OnSeekBarChangeListener {

        @Override
        public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
            int position = (Integer) seekBar.getTag();
            if (mListener != null) {
                mListener.onProgressChanged(seekBar, progress, fromUser, position);
            }
        }

        @Override
        public void onStartTrackingTouch(SeekBar seekBar) {
            int position = (Integer) seekBar.getTag();
            if (mListener != null) {
                mListener.onStartTrackingTouch(seekBar, position);
            }
        }

        @Override
        public void onStopTrackingTouch(SeekBar seekBar) {
            int position = (Integer) seekBar.getTag();
            if (mListener != null) {
                mListener.onStopTrackingTouch(seekBar, position);
            }
        }

    }
}