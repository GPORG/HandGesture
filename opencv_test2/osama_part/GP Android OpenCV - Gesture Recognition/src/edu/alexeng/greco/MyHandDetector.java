package edu.alexeng.greco;

import java.io.File;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.MatOfInt;
import org.opencv.core.MatOfPoint;
import org.opencv.core.MatOfPoint2f;
import org.opencv.core.Point;
import org.opencv.core.Rect;
import org.opencv.core.RotatedRect;
import org.opencv.core.Scalar;
import org.opencv.core.Size;
import org.opencv.imgproc.Imgproc;
import org.opencv.ml.CvSVM;

import android.content.Context;
import android.util.Log;

public class MyHandDetector {

	private List<MatOfPoint> contours;
	private RotatedRect handBoundary;
	private int largestContourIndex;
	private String currentGesture;
	private String prevGesture;
	private boolean takeActions;
	private Mat hand, temp;
	private Mat frameInProcess;
	private MatOfPoint largestContour;
	private int actionTime;
	private final int gridSize = 10;
	private float[] feature;
	private CvSVM SVM;

	public MyHandDetector(Context c) {
		// some ini
		contours = new ArrayList<MatOfPoint>();
		handBoundary = new RotatedRect();
		largestContourIndex = -1;
		currentGesture = "";
		prevGesture = "";
		takeActions = false;

		frameInProcess = new Mat();
		actionTime = 1;
		feature = new float[100];
		SVM = new CvSVM();
		// load classifier model
		// ==========================================
		try {
			loadXmlFile(c);
		} catch (Exception e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		// ==========================================
		Log.d("koko", "" + SVM.get_var_count());
	}

	public void process(Mat mFrame) {
		Imgproc.GaussianBlur(mFrame, frameInProcess, new Size(5, 5), 0, 0);
		// Imgproc.erode(frameInProcess, frameInProcess, new Mat());
		// Imgproc.dilate(frameInProcess, frameInProcess, new Mat());
		Imgproc.cvtColor(frameInProcess, frameInProcess,
				Imgproc.COLOR_BGR2YCrCb);
		Core.inRange(frameInProcess, new Scalar(0, 133, 77), new Scalar(255,
				173, 127), frameInProcess);
		Imgproc.medianBlur(frameInProcess, frameInProcess, 5);
		hand = frameInProcess.clone();
		// finding contours
		Imgproc.findContours(frameInProcess, contours, new Mat(),
				Imgproc.RETR_LIST, Imgproc.CHAIN_APPROX_SIMPLE);

		// finding largest contour
		largestContourIndex = -1;
		largestContour = null;
		double maxArea = 0;
		Iterator<MatOfPoint> each = contours.iterator();
		for (int i = 0; each.hasNext(); i++) {
			MatOfPoint wrapper = each.next();
			double area = Imgproc.contourArea(wrapper);
			if (area > maxArea) {
				largestContourIndex = i;
				largestContour = wrapper;
				maxArea = area;
			}
		}
		if (largestContourIndex == -1 || largestContour == null
				|| largestContour.total() == 0)
			return;
		/*
		 * drawing
		 */

		MatOfInt hull = new MatOfInt();
		Imgproc.convexHull(largestContour, hull);
		// draw convex hull of the largest contour
		Point p0 = new Point(largestContour.get((int) hull.get(0, 0)[0], 0));
		for (int i = 1; i < hull.rows(); i++) {
			Point pi = new Point(largestContour.get((int) hull.get(i, 0)[0], 0));
			Core.line(mFrame, p0, pi, new Scalar(0, 255, 0));
			p0 = pi;
		}

		handBoundary = Imgproc.minAreaRect(new MatOfPoint2f(largestContour
				.toArray()));
		// copy the hand in its own image
		double max = handBoundary.size.width;
		if (max < handBoundary.size.height)
			max = handBoundary.size.height;
		double left = handBoundary.center.x - (max / 2) - 25, top = handBoundary.center.y
				- (max / 2) - 25;
		double width = max + 50, hight = max + 50;
		if (left < 0)
			left = 0;
		if (top < 0)
			top = 0;
		if ((left + width) > hand.width())
			width = hand.width() - left;
		if ((top + hight) > hand.height())
			hight = hand.height() - top;
		if (width <= 0 || hight <= 0) {
			left = 0;
			top = 0;
			width = hand.width();
			hight = hand.height();
		}
		Rect rounded = new Rect(new double[] { left, top, width, hight });

		hand = hand.submat(rounded);

		// extract features then detect.
		int label = labelGesture();
		String gName = "";
		switch (label) {
		case 1:
			gName = "up";
			break;
		case 2:
			gName = "open";
			break;
		case 3:
			gName = "capture";
			break;
		case 4:
			gName = "call";
			break;
		case 5:
			gName = "left";
			break;
		case 6:
			gName = "right";
			break;
		case 7:
			gName = "closed";
			break;
		case 8:
			gName = "start";
			break;

		default:
			gName = "None";
			break;
		}
		Core.putText(mFrame, gName, new Point(30, 30), 1, 1, new Scalar(145,
				35, 100));
		// take the action
		if (actionTime % 20 == 0)
			applyAction(gName);
		actionTime++;

	}

	private void extractFeature() {

		int unitWidth = hand.cols() / gridSize;
		int unitHeight = hand.rows() / gridSize;
		int index = 0;
		for (int i = 0; i < gridSize; i++) {
			for (int j = 0; j < gridSize; j++) {
				Mat subImg = new Mat(hand, new Rect(new double[] {
						j * unitWidth, i * unitHeight, unitWidth, unitHeight }));
				int white = Core.countNonZero(subImg);
				if (white > 0)
					feature[index] = 0;
				else
					feature[index] = 1;
				index++;
				subImg.release();
			}
		}
	}

	private int labelGesture() {
		extractFeature();
		Mat test = new Mat(1, 100, CvType.CV_32FC1);
		test.put(0, 0, feature);
		return (int) SVM.predict(test);
	}

	// here we must do the actual action
	private void applyAction(String gestureName) {

		if (gestureName.compareTo("start") == 0) {
			// check prev gesture
			if (prevGesture.compareTo("") == 0) {// so it is the first time to
													// use start gesture
				currentGesture = "start";
				prevGesture = "start";
				takeActions = true;
				Log.d("cv", "start");
			} else if (prevGesture.compareTo("closed") == 0) {
				// here start gesture comes after the separator...then don't
				// apply any actions
				prevGesture = "";
				currentGesture = "";
				takeActions = false;
				Log.d("cv", "stop");
			} else {
				// don't take any actions
			}
		} else if (gestureName.compareTo("closed") == 0) {
			// check if we are working or not
			if (takeActions) {
				prevGesture = "closed";
				Log.d("cv", "applying closed");
			}
		} else if (gestureName.compareTo("open") == 0) {
			if (prevGesture.compareTo("closed") == 0) {
				Log.d("cv", "apply open action");
				prevGesture = "open";
			}
		} else if (gestureName.compareTo("capture") == 0) {
			if (prevGesture.compareTo("closed") == 0) {
				Log.d("cv", "apply capture action");
				prevGesture = "capture";
			}
		} else if (gestureName.compareTo("call") == 0) {
			if (prevGesture.compareTo("closed") == 0) {
				Log.d("cv", "apply call action");
				prevGesture = "call";
			}
		} else if (gestureName.compareTo("up") == 0) {
			if (prevGesture.compareTo("closed") == 0) {
				Log.d("cv", "apply up action");
				prevGesture = "up";
			}
		} else if (gestureName.compareTo("left") == 0) {
			if (prevGesture.compareTo("closed") == 0) {
				Log.d("cv", "apply left action");
				prevGesture = "left";
			}
		} else if (gestureName.compareTo("right") == 0) {
			if (prevGesture.compareTo("closed") == 0) {
				Log.d("cv", "apply right action");
				prevGesture = "right";
			}
		}
	}

	private void loadXmlFile(Context c) throws Exception {
		InputStream in = c.getResources().openRawResource(R.raw.dataset);
		File dir = c.getDir("dataSet", Context.MODE_PRIVATE);
		File file = new File(dir, "AndroidDataset.xml");
		FileOutputStream out = new FileOutputStream(file);
		byte[] buffer = new byte[4096];
		int byteRead;
		while ((byteRead = in.read(buffer)) != -1)
			out.write(buffer, 0, byteRead);
		in.close();
		out.close();

		SVM.load(file.getAbsolutePath());
	}
}
