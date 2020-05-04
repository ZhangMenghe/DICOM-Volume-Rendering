package helmsley.vr.DUIs;

import android.content.Context;
import android.graphics.Typeface;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.TextView;

import java.lang.ref.WeakReference;
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

    public List<String> getItem_names(){return item_names;}
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
    public void addItem(){}
    public void deleteItem(int id){}
    public void Reset(){}

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
    void setTitleByText(String title) {
        if(!item_names.contains(title)) setTitleById(0);
        else this.title = title;notifyDataSetChanged();
    }
    void setTitleById(int id){
        if(id<item_names.size()){
            this.title = item_names.get(id);
            notifyDataSetChanged();
        }
    }
    public View getView(int position, View convertView, ViewGroup parent){
        return getViewWithText(convertView, title, -1);
    }
    public View getDropDownView(int position, View convertView, ViewGroup parent) {
        return getViewWithText(convertView, item_names.get(position), position);
    }
    void onItemClick(int position){
        Log.e("TAG", "===onItemClick: " + position );
    }
    void onItemClick(String text_title_content){onItemClick(item_names.indexOf(text_title_content));}
    View getViewWithText(View convertView, String content, int position){
        ViewTitleHolder holder;

        if (convertView == null) {
            holder = new ViewTitleHolder();
            convertView = mInflater.inflate(R.layout.thin_spinner_item, null);
            holder.text_title = (TextView) convertView.findViewById(R.id.titleName);
            if(position >= 0)
                holder.text_title.setOnClickListener(new View.OnClickListener(){
                    @Override
                    public void onClick(View v) {
                        String text_title_content = ((TextView)v).getText().toString();
                        title = text_title_content;
                        onItemClick(text_title_content);
                        notifyDataSetChanged();
                    }
                } );
            else
                holder.text_title.setTypeface(null, Typeface.BOLD);
            convertView.setTag(R.layout.thin_spinner_item, holder);
        } else {
            holder = (ViewTitleHolder) convertView.getTag(R.layout.thin_spinner_item);
        }
        holder.text_title.setText(content);
        return convertView;
    }
}



