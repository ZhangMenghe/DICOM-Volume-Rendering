package helmsley.vr.DUIs;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.TextView;

import java.lang.ref.WeakReference;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

import helmsley.vr.R;

public abstract class ListAdapter extends BaseAdapter {
    LayoutInflater mInflater;
    String title;
    final WeakReference<Context> contexRef;
    List<String> item_names;
    ListAdapter(Context context, String title) {
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
class textSimpleListAdapter extends ListAdapter {
    textSimpleListAdapter(Context context, int arrayId) {
        super(context, "");
        item_names = Arrays.asList(context.getResources().getStringArray(arrayId));
    }
    textSimpleListAdapter(Context context, List<String>items) {
        super(context, "");
        item_names = items;
    }
    void setTitleByText(String title) {this.title = title;}
    void setTitleById(int id){if(id<item_names.size())this.title = item_names.get(id);}
    public View getView(int position, View convertView, ViewGroup parent){
        return getViewWithText(convertView, title, false);
    }
    public View getDropDownView(int position, View convertView, ViewGroup parent) {
        return getViewWithText(convertView, item_names.get(position), true);
    }
    private View getViewWithText(View convertView, String content, boolean is_item){
        ViewTitleHolder holder;

        if (convertView == null) {
            holder = new ViewTitleHolder();
            convertView = mInflater.inflate(R.layout.thin_spinner_item, null);
            holder.text_title = (TextView) convertView.findViewById(R.id.titleName);
            if(is_item)
                holder.text_title.setOnClickListener(new View.OnClickListener(){
                    @Override
                    public void onClick(View v) {
                        title = ((TextView)v).getText().toString();
                        notifyDataSetChanged();
                    }
                } );
            convertView.setTag(R.layout.thin_spinner_item, holder);
        } else {
            holder = (ViewTitleHolder) convertView.getTag(R.layout.thin_spinner_item);
        }
        holder.text_title.setText(content);
        return convertView;
    }
}



