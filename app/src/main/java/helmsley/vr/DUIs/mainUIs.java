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

import java.io.FileWriter;
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
    private String cached_yaml = null;

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

        dialogController = new dialogUIs(activity, this, parent_view);

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

    void LoadConfig(String content){
        Yaml yloader = new Yaml();
        try{
//            InputStream ins = actRef.get().getAssets().open("config.yml");
            mUIManagerRef.get().RequestResetWithTemplate((LinkedHashMap)yloader.load(content));
        }catch (Exception e){
            Log.e(TAG, "===fail to load yaml===");
            e.printStackTrace();
        }
    }
    String getExportConfig(String name, String comments){
        return cache_current_state_as_yaml(name, comments);
    }
    private String cache_current_state_as_yaml(String name, String comments){
        //todo:ui that choose a name sth..
        String content = null;
        //construct a hash-table-type structure that contains all the information and write to yaml
        Yaml ysaver = new Yaml();
        try{
//            String fsys_root = actRef.get().getFilesDir().getAbsolutePath();
//            FileWriter writer = new FileWriter(fsys_root + "/config.yml");
            LinkedHashMap map = new LinkedHashMap();
            map.put("name", name);
            if(!comments.isEmpty())map.put("comments", comments);
            mUIManagerRef.get().getCurrentStates(map);
            content = ysaver.dump(map);
        }catch (Exception e){
            Log.e(TAG, "===fail to save yaml===");
            e.printStackTrace();
        }
        return content;
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
                                else if(text_title.equals(NAME_DATA_REMOTE))dialogController.ShowDatasetRemote();
                                else if(text_title.equals(NAME_TEMPLATE_LOAD))dialogController.ShowConfigsRemote();
                                else if(text_title.equals(NAME_TEMPLATE_SAVE))dialogController.ExportConfigs();
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
