package helmsley.vr.DUIs;

import android.app.Activity;
import android.content.Context;
import android.content.res.Resources;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Spinner;
import android.widget.TextView;

import org.yaml.snakeyaml.Yaml;

import java.io.InputStream;
import java.lang.ref.WeakReference;
import java.util.Arrays;
import java.util.LinkedHashMap;
import java.util.Map;

import helmsley.vr.R;
import helmsley.vr.UIsManager;
import helmsley.vr.Utils.ConfigObject;

public class mainUIs {
    final static String TAG = "mainUIs";
    private final WeakReference<Activity> actRef;
    private final WeakReference<UIsManager> mUIManagerRef;
    private final String NAME_RESET, NAME_TEMPLATE_SAVE, NAME_TEMPLATE_LOAD, NAME_DATA_REMOTE, NAME_DATA_LOCAL;

    private Spinner spinner_check;
    private dialogUIs dialogController;

    //Spinner adapter
    private checkpanelAdapter cb_panel_adapter;

    public mainUIs(final Activity activity, final ViewGroup parent_view, UIsManager manager){
        mUIManagerRef = new WeakReference<>(manager);
        actRef = new WeakReference<>(activity);
        //checkbox spinners
        spinner_check =  (Spinner)activity.findViewById(R.id.checkPanelSpinner);
        cb_panel_adapter = new checkpanelAdapter(activity, manager);
        //function spinners
        Spinner spinner_func = (Spinner) activity.findViewById(R.id.funcSpinner);
        syscallListAdapter fAdapter = new syscallListAdapter(activity, R.array.functions, R.string.sys_name);
        spinner_func.setAdapter(fAdapter);

        dialogController = new dialogUIs(activity, parent_view);

        Resources res = activity.getResources();
        NAME_RESET = res.getString(R.string.sys_reset);
        NAME_TEMPLATE_SAVE = res.getString(R.string.sys_template_save);NAME_TEMPLATE_LOAD = res.getString(R.string.sys_template_load);
        NAME_DATA_REMOTE = res.getString(R.string.sys_data_remote); NAME_DATA_LOCAL = res.getString(R.string.sys_data_local);
    }

    public void Reset(){
        cb_panel_adapter.Reset();
        spinner_check.setAdapter(cb_panel_adapter);
    }

    public boolean[] getPanelStatus(){return cb_panel_adapter.getAllValues();}

    public void updateOnFrame(){dialogController.updateOnFrame();}

    private void loadTemplate(){
        //todo:
        //show a dialog to select template

        //YAML Loader
        Yaml yloader = new Yaml();
        try{
            InputStream ins = actRef.get().getAssets().open("config.yml");
            mUIManagerRef.get().RequestResetWithTemplate((LinkedHashMap)yloader.load(ins));
        }catch (Exception e){
            Log.e(TAG, "===fail to load yaml===");
            e.printStackTrace();
        }
    }
    private void saveTemplate(){

    }

    private class syscallListAdapter extends ListAdapter{
        syscallListAdapter(Context context, int arrayId, int titleId){
                super(context, context.getResources().getString(titleId));
                item_names = Arrays.asList(context.getResources().getStringArray(arrayId));

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

                holder.text_name.setOnTouchListener(new View.OnTouchListener(){
                    public boolean onTouch(View view, MotionEvent event) {
                        switch (event.getActionMasked()) {
                            case MotionEvent.ACTION_DOWN:
                                holder.text_name.setTextAppearance(R.style.itemHighlightText);
                                String text_title = ((TextView)view).getText().toString();
                                if(text_title.equals(NAME_RESET)) mUIManagerRef.get().RequestReset();
                                else if(text_title.equals(NAME_DATA_LOCAL))dialogController.SetupConnectLocal();
                                else if(text_title.equals(NAME_DATA_REMOTE))dialogController.SetupConnectRemote();
                                else if(text_title.equals(NAME_TEMPLATE_LOAD))loadTemplate();
                                else if(text_title.equals(NAME_TEMPLATE_SAVE))saveTemplate();
                                break;
                            case MotionEvent.ACTION_UP:
                                holder.text_name.setTextAppearance(R.style.itemText);
                                break;

                            default:
                                break;
                        }
                        return true;
                    }

                });


            return convertView;
        }
        private class ViewContentHolder{
            TextView text_name;
        }
    }
}
