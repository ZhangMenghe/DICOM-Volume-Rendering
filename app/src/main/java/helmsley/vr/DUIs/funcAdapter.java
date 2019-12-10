package helmsley.vr.DUIs;

import android.content.Context;
import android.util.Log;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import java.lang.ref.WeakReference;
import java.util.ArrayList;
import java.util.Arrays;

import helmsley.vr.R;
import helmsley.vr.UIsManager;

public class funcAdapter{
    private WeakReference<funcListAdapter> mAdapterRef = null;

    private final WeakReference<UIsManager> mUIManagerRef;
    private final WeakReference<Context> contexRef;

    public funcAdapter(Context context, UIsManager manager){
        contexRef = new WeakReference<>(context);
        mUIManagerRef = new WeakReference<>(manager);
    }
    public funcListAdapter createFuncAdapter(){
        if(mAdapterRef == null) mAdapterRef = new WeakReference<>(new funcListAdapter(
                contexRef.get(),
                contexRef.get().getResources().getStringArray(R.array.functions),
                contexRef.get().getString(R.string.fund_group_name)));
        return mAdapterRef.get();
    }
    public class funcListAdapter extends ListAdapter{
        public funcListAdapter(Context context, String[] item_names, String title){
            super(context, title);
            this.item_names = new ArrayList<>(Arrays.asList(item_names));
        }
        public View getDropDownView(int position, View convertView, ViewGroup parent){
            ViewContentHolder holder;
            if (convertView == null) {
                holder = new ViewContentHolder();
                convertView = mInflater.inflate(R.layout.spinner_item, null);
                holder.text_name = (TextView) convertView.findViewById(R.id.funcName);
                convertView.setTag(R.layout.spinner_check_layout, holder);
            } else {
                holder = (ViewContentHolder) convertView.getTag(R.layout.spinner_check_layout);
            }
            holder.text_name.setText(item_names.get(position));
            holder.text_name.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    switch (position){
                        case 0:onClickResetButton();break;
                        case 1:onClickDataChangeButton();break;
                        default:break;
                    }
                }
            });

            return convertView;
        }
        private class ViewContentHolder{
            TextView text_name;
        }
    }
    private void onClickDataChangeButton(){
        mUIManagerRef.get().RequestSetupServerConnection();
        //todo: unfinish!
    }
    private void onClickResetButton(){
    }
}

