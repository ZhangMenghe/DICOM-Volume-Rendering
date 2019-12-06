package helmsley.vr.DUIs;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.TextView;

import java.lang.ref.WeakReference;
import java.util.ArrayList;

import helmsley.vr.R;

public abstract class ListAdapter extends BaseAdapter {
    protected LayoutInflater mInflater;

    protected String title;
    protected final WeakReference<Context> contexRef;

    protected ArrayList<String> item_names;


    public ListAdapter(Context context, String title) {
        contexRef = new WeakReference<>(context);
        mInflater = LayoutInflater.from(context);
        this.title = title;
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
        ViewTitleHolder holder;

        if (convertView == null) {
            holder = new ViewTitleHolder();
            convertView = mInflater.inflate(R.layout.baseadapter_layout, null);
            holder.text_title = (TextView) convertView.findViewById(R.id.nameHeader);
            convertView.setTag(R.layout.baseadapter_layout, holder);
        } else {
            holder = (ViewTitleHolder) convertView.getTag(R.layout.baseadapter_layout);
        }
        holder.text_title.setText(title);
        return convertView;
    }

    static class ViewTitleHolder {
        TextView text_title;
    }
}

