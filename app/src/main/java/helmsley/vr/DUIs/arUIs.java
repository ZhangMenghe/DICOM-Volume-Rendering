package helmsley.vr.DUIs;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.Context;
import android.content.res.Resources;
import android.content.res.TypedArray;
import android.graphics.Color;
import android.support.constraint.ConstraintLayout;
import android.support.design.widget.FloatingActionButton;
import android.util.DisplayMetrics;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.MenuItem;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.PopupMenu;
import android.widget.Spinner;
import android.widget.TextView;

import java.lang.ref.WeakReference;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.LinkedHashMap;
import java.util.List;

import helmsley.vr.R;
import helmsley.vr.UIsManager;

public class arUIs extends BasePanel{
    private static WeakReference<arUIs> selfRef;
    private Spinner spinner_check_render;
    private CheckBox check_pointer;
    //Spinner adapter
    private checklistAdapter cb_adapter;
    private PopupMenu pop_menu;
    private View pop_parent;
    private View bottom_panel;

    public arUIs(final Activity activity, ViewGroup parent_view) {
        super(activity, parent_view);
        selfRef = new WeakReference<>(this);
        final LayoutInflater mInflater = LayoutInflater.from(activity);

        View panel_ = mInflater.inflate(R.layout.ar_panel, parent_view, false);
        spinner_check_render =  (Spinner)panel_.findViewById(R.id.check_render_spinner);
        cb_adapter = new checklistAdapter(activity);

        sub_panels_.add(panel_);
        setup_checks(
                panel_,
                R.array.ar_check_params, R.array.ar_check_values,
                R.id.check_ar_enabled, 0);

        //bottom panel
        bottom_panel = mInflater.inflate(R.layout.ar_bottom_panel, parent_view, false);
        sub_panels_.add(bottom_panel);
        FloatingActionButton fab = bottom_panel.findViewById(R.id.action_button);

        fab.setOnTouchListener(new View.OnTouchListener(){
            public boolean onTouch(View view, MotionEvent event) {
                float vx = -1.0f, vy =-1.0f;
                  switch (event.getActionMasked()) {
                    case MotionEvent.ACTION_DOWN:
                        fab.setAlpha(0.5f);
                        if(vx < 0 ){
                            vx = view.getX() + view.getWidth() * 0.5f;
                            vy = view.getY() + view.getHeight()*0.5f;
                        }
                        JUIInterface.JUIonSingleTouchDown(1, vx, vy);

//                        JUIInterface.JUIonSingleTouchDown(1, event.getX(), event.getY());
                        break;

                    case MotionEvent.ACTION_MOVE:
                        break;

                    case MotionEvent.ACTION_UP:
                        fab.setAlpha(1.f);
                        JUIInterface.JUIonSingleTouchUp();
                        break;
                    case MotionEvent.ACTION_BUTTON_PRESS:
                    default:
                        return false;
                }
                return true;
            }

        });
        bottom_panel.setVisibility(View.GONE);

        check_pointer = (CheckBox)panel_.findViewById(R.id.check_float_widget);
        check_pointer.setChecked(false);
        check_pointer.setOnCheckedChangeListener(new CheckBox.OnCheckedChangeListener(){
            @Override
            public void onCheckedChanged(CompoundButton buttonView,
                                         boolean isChecked) {
                if(isChecked) bottom_panel.setVisibility(View.VISIBLE);
                else bottom_panel.setVisibility(View.GONE);
            }
        });
        setup_pop_dialog(activity, parent_view);
    }
    private void setup_pop_dialog(Activity activity, ViewGroup parent_view){
        pop_parent = new View(activity);
        pop_parent.setLayoutParams(new ViewGroup.LayoutParams(1, 1));
        pop_parent.setBackgroundColor(Color.TRANSPARENT);

        parent_view.addView(pop_parent);

        pop_menu = new PopupMenu(activity, pop_parent, Gravity.CENTER);
        pop_menu.getMenuInflater().inflate(R.menu.popup_menu, pop_menu.getMenu());
        pop_menu.setOnMenuItemClickListener(new PopupMenu.OnMenuItemClickListener() {
            @Override
            public boolean onMenuItemClick(MenuItem item) {
                switch (item.getItemId()){
                    case R.id.place_volume:
                        JUIInterface.JUIonARRequest(0);
                        return true;
                    case R.id.place_anchor:
                        JUIInterface.JUIonARRequest(1);
                        return true;
                    default:
                        return false;
                }
            }
        });
    }
    @Override
    public void Reset(){
        spinner_check_render.setAdapter(cb_adapter);
        primary_checkbox.setChecked(default_primary_check);
        check_pointer.setChecked(false);
//        bottom_panel.setVisibility(View.GONE);
    }
    public void ResetWithTemplate(LinkedHashMap map, ArrayList<String> names, ArrayList<Boolean> values){
        LinkedHashMap maskmap = (LinkedHashMap) map.getOrDefault("mask", null);
        if(maskmap == null) return;
        boolean status = (Boolean) maskmap.getOrDefault("status", default_primary_check);
        primary_checkbox.setChecked(status);
    }
    public LinkedHashMap getCurrentStates(){
        LinkedHashMap map = new LinkedHashMap();
        map.put("status", primary_checkbox.isChecked());
        map.put("Points", cb_adapter.getValue(0));
        map.put("Planes", cb_adapter.getValue(1));
        return map;
    }
    @Override
    public void showHidePanel(boolean isPanelOn){
        super.showHidePanel(isPanelOn);
    }

    private static class checklistAdapter extends ListAdapter {
        List<Boolean> item_values;
        checklistAdapter(Context context) {
            super(context, context.getString(R.string.ar_draw_name));
            //setup values
            Resources res = context.getResources();
            item_names = Arrays.asList(res.getStringArray(R.array.ar_render_params));
            TypedArray check_values = res.obtainTypedArray(R.array.ar_check_values);
            item_values = new ArrayList<>();
            for (int i = 0; i < item_names.size(); i++) item_values.add(check_values.getBoolean(i+1, false));
            check_values.recycle();
        }
        void setValue(int id, boolean value){
            if(id < item_values.size() && item_values.get(id)!=value) {
                item_values.set(id, value);
                notifyDataSetChanged();
            }
        }
        boolean getValue(int id){return (id<item_values.size())?item_values.get(id):false;}
        public View getDropDownView(int position, View convertView, ViewGroup parent){
            checklistAdapter.ViewContentHolder holder;
            if (convertView == null) {
                holder = new ViewContentHolder();
                convertView = mInflater.inflate(R.layout.spinner_check_layout, null);
                holder.text_name = (TextView) convertView.findViewById(R.id.checkName);
                holder.checkBox = (CheckBox) convertView.findViewById(R.id.checkCheckBox);

                convertView.setTag(R.layout.spinner_check_layout, holder);

            } else {
                holder = (checklistAdapter.ViewContentHolder) convertView.getTag(R.layout.spinner_check_layout);
            }
            holder.text_name.setText(item_names.get(position));
            holder.checkBox.setOnCheckedChangeListener(new CheckBox.OnCheckedChangeListener(){
                @Override
                public void onCheckedChanged(CompoundButton buttonView,
                                             boolean isChecked) {
                    if(item_values.get(position) == isChecked) return;
                    item_values.set(position, isChecked);
                    JUIInterface.JUIsetChecks(item_names.get(position), isChecked);
                }
            });
            holder.checkBox.setTag(position);
            holder.checkBox.setChecked(item_values.get(position));
            return convertView;
        }
        static class ViewContentHolder{
            TextView text_name;
            CheckBox checkBox;
        }
    }
    //called from jni
    public static void ShowPopMenu(float x, float y){
        arUIs ui = selfRef.get();
        ui.pop_parent.setX(x);
        ui.pop_parent.setY(y);
        ui.pop_menu.show();
    }
}
