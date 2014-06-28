package HelperClasses;

import java.io.File;
import java.util.ArrayList;
import java.util.Scanner;

import android.os.Environment;
import android.util.Log;

public class FileReader {

	private Scanner sc;
	private ArrayList<String> imgs;

	public FileReader() {
		String  root = Environment.getExternalStorageDirectory().getPath();
		File filePath=new File(root+"//paths.txt");
		try {
			imgs = new ArrayList<String>();
			Scanner sc = new Scanner(filePath);
			while (sc.hasNextLine())
				imgs.add(sc.nextLine());
			sc.close();
		} catch (Exception e) {
			Log.d("error", "error in reading img paths");
		}

	}

	public ArrayList<String> getImgs() {
		return imgs;
	}
}
