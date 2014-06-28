package HelperClasses;

import java.io.File;

import android.os.Environment;
import android.util.Log;

public class FileWriter {

	java.io.FileWriter out;

	public FileWriter(float[][] data) {
		try{
		out = new java.io.FileWriter(new File(Environment
				.getExternalStorageDirectory().getPath()) + "//out.txt");
	
		for(int i=0;i<data.length;i++){
			for(int j=0;j<data[0].length;j++){
				out.write(""+(int)data[i][j]);
			}
			out.write("\n");
		}
		out.close();
		}catch (Exception e) {
		Log.d("Error", "error in writing");
	}
		}
}
