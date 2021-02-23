package helmsley.vr.Utils;

import android.content.res.AssetManager;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Matrix;
import android.os.Environment;
import android.util.Log;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.FileWriter;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.nio.file.StandardOpenOption;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.List;

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

     public static boolean deleteDirectory(File path) {
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
    public static ArrayList<Bitmap> loadImagesFromDir(String dir_path_str, boolean flip_vertical){
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

                    Bitmap mask = BitmapFactory.decodeStream(fis);
                    if(flip_vertical){
                        Matrix matrix = new Matrix();
                        matrix.preScale(1.0f, -1.0f);
                        mask = Bitmap.createBitmap(mask, 0, 0, mask.getWidth(), mask.getHeight(), matrix, true);
                    }
                    img_list.add(mask);
                }catch (IOException e){
                    Log.e(TAG, "===load_mask error from : " + dir_path_str);
                }
            }
        }
        ArrayList<Bitmap> sorted_img_list = new ArrayList(img_list);
        Collections.sort(sorted_img_list, Comparator.comparing(s -> name_list.get(img_list.indexOf(s))));
        return sorted_img_list;
    }
    public static List<String> readLines(String filename){
        List<String> allLines = new ArrayList<>();
        try {
            allLines = Files.readAllLines(Paths.get(filename));
        } catch (IOException e) {
            e.printStackTrace();
        }
        return allLines;
    }
    public static void addToFile(String filename, String content){
        try{
            Path path = Paths.get(filename);
            Files.write(path, content.getBytes(), StandardOpenOption.APPEND);
        }catch (IOException e) {
            e.printStackTrace();
        }
    }
    public static void writeToFile(String filename, List<String> lines){
        try{
            File cf = new File(filename);
            if(cf.exists() && cf.isFile()) cf.delete();
            FileWriter writer = new FileWriter(filename);
            for(String str: lines) {
                writer.write(str + System.lineSeparator());
            }
            writer.close();
        }catch (IOException e) {
            e.printStackTrace();
        }
    }
    public static void saveLargeImageToFile(OutputStream ostream, byte[] data){
        try{
            ostream.write(data);
            ostream.flush();
            ostream.close();
        }catch (IOException e){
            e.printStackTrace();
            Log.e(TAG, "====Failed to Save Large Image to file");
        }
    }
    public static void writeFileToExternalStorage(byte[] data){
//        String cashback = "Get 2% cashback on all purchases from xyz \n Get 10% cashback on travel from dhhs shop";
        String state = Environment.getExternalStorageState();
        //external storage availability check
        if (!Environment.MEDIA_MOUNTED.equals(state)) {
            return;
        }
        File file = new File(Environment.getExternalStoragePublicDirectory(
                Environment.DIRECTORY_DOWNLOADS), "mchessboard");

        FileOutputStream outputStream = null;
        try {
            file.createNewFile();
            //second argument of FileOutputStream constructor indicates whether to append or create new file if one exists
            outputStream = new FileOutputStream(file, true);

            outputStream.write(data,0,480*640);//cashback.getBytes());
            outputStream.flush();
            outputStream.close();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}

