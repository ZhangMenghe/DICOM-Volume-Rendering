package helmsley.vr.Utils;

import android.content.res.AssetManager;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.util.Log;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.Comparator;
import java.util.LinkedList;
import java.util.List;
import java.util.Queue;

public class fileUtils {
    final static String TAG = "File Utils";
    public static void copyFromAsset(AssetManager assetManager, String src_path, String target_path)
            throws IOException{
        String assets[] = assetManager.list(src_path);
        if(assets.length == 0){
            copyAssetFile(assetManager, src_path, target_path);
        }else{
            File dir = new File(target_path);
            if(!dir.exists())
                dir.mkdir();
            for(int i=0; i<assets.length; i++){
                String nsrc = src_path + "/" + assets[i];
                String ntarget = target_path + "/" + assets[i];
                copyFromAsset(assetManager, nsrc, ntarget);
            }
        }
    }
    public static void copyDirectory(File sourceLocation, File targetLocation)
            throws IOException {

        if (sourceLocation.isDirectory()) {
            if (!targetLocation.exists()) {
                targetLocation.mkdirs();
            }

            String[] children = sourceLocation.list();
            for (int i = 0; i < children.length; i++) {
                copyDirectory(new File(sourceLocation, children[i]), new File(
                        targetLocation, children[i]));
            }
        } else {

            copyFile(sourceLocation, targetLocation);
        }
    }

    /**
     * @param sourceLocation
     * @param targetLocation
     * @throws FileNotFoundException
     * @throws IOException
     */
    public static void copyFile(File sourceLocation, File targetLocation)
            throws FileNotFoundException, IOException {
        InputStream in = new FileInputStream(sourceLocation);
        OutputStream out = new FileOutputStream(targetLocation);

        // Copy the bits from instream to outstream
        byte[] buf = new byte[1024];
        int len;
        while ((len = in.read(buf)) > 0) {
            out.write(buf, 0, len);
        }
        in.close();
        out.close();
    }

    public static void copyAssetFile(AssetManager assetManager, String src_name, String dest_name){
        File model = new File(dest_name);
        try {
            if(!model.exists()){
                InputStream is = assetManager.open(src_name);
                byte[] buffer = new byte[is.available()];
                is.read(buffer);
                FileOutputStream os = new FileOutputStream(model);
                os.write(buffer);
            }
        } catch (Exception e) {
            Log.e(TAG, "===copyAssetFile: fail to copy from asset"+ e);
        }
    }

    static public boolean deleteDirectory(File path) {
        if (path.exists()) {
            File[] files = path.listFiles();
            for (int i = 0; i < files.length; i++) {
                if (files[i].isDirectory()) {
                    deleteDirectory(files[i]);
                } else {
                    files[i].delete();
                }
            }
        }
        return (path.delete());
    }
    public static List<String> getListFilesFromDir(File path) {
        ArrayList<String> file_path_lst = new ArrayList<String>();
        if (path.exists()) {
            File[] files = path.listFiles();
            for(File file:files){
                if(file.isDirectory())
                    file_path_lst.addAll(getListFilesFromDir(file));
                else file_path_lst.add(file.getAbsolutePath());
            }
        }
        return file_path_lst;
    }
    public static ArrayList<Bitmap> loadImagesFromDir(String dir_path_str){
        ArrayList<Bitmap> img_list = new ArrayList<>();
        ArrayList<Integer> name_list = new ArrayList<>();
        int dir_len = dir_path_str.length();

        File dir_path = new File(dir_path_str);
        if(!dir_path.exists()) return img_list;

        if (dir_path.exists()) {
            File[] files = dir_path.listFiles();
            for(File file:files){
                String full_name = file.getAbsolutePath();
                name_list.add(Integer.parseInt(full_name.substring(dir_len+1, full_name.length()-4)));
                try{
                    FileInputStream fis = new FileInputStream(full_name);
                    img_list.add(BitmapFactory.decodeStream(fis));
                }catch (IOException e){
                    Log.e(TAG, "===load_mask error from : " + dir_path_str);
                }
            }
        }
        ArrayList<Bitmap> sorted_img_list = new ArrayList(img_list);
        Collections.sort(sorted_img_list, Comparator.comparing(s -> name_list.get(img_list.indexOf(s))));
        return sorted_img_list;
    }
}

