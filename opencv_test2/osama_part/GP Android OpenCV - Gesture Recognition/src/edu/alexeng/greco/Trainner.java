package edu.alexeng.greco;

import java.io.File;
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
import org.opencv.core.TermCriteria;
import org.opencv.highgui.Highgui;
import org.opencv.imgproc.Imgproc;
import org.opencv.ml.CvSVM;
import org.opencv.ml.CvSVMParams;

import HelperClasses.FileReader;
import HelperClasses.FileWriter;
import android.os.Environment;
import android.util.Log;

public class Trainner {

	private List<MatOfPoint> contours;
	private RotatedRect handBoundary;
	private int largestContourIndex;
	private Mat hand, temp;
	private Mat frameInProcess;
	private MatOfPoint largestContour;
	private final int gridSize = 10;
	private final int dataSetSize = 177;
	private float[][] data;
	private float[] labels;
	private CvSVM SVM;
	private int row;

	private FileReader reader;

	public Trainner() {
		// some ini
		contours = new ArrayList<MatOfPoint>();
		handBoundary = new RotatedRect();
		largestContourIndex = -1;
		frameInProcess = new Mat();
		data = new float[dataSetSize][100];
		labels = new float[dataSetSize];
		boolean loop = false;
		row = 0;
		reader = new FileReader();
		ArrayList<String> paths = reader.getImgs();
		String root = Environment.getExternalStorageDirectory().getPath()
				+ "//";
		for (int i = 0; i < (paths.size() - 1); i++) {
			String path = root + paths.get(i);
			Mat img = Highgui.imread(path);
			process(img);
		}
		for (int j = 0; j < 100; j++)
			data[row][j] = 0;

		train();
		FileWriter writer = new FileWriter(data);
		Log.d("hamada", "finishhhhhhhhhh");

	}

	public void process(Mat mFrame) {
		Imgproc.GaussianBlur(mFrame, frameInProcess, new Size(5, 5), 0, 0);
		// Imgproc.erode(frameInProcess, frameInProcess, new Mat());
		// Imgproc.dilate(frameInProcess, frameInProcess, new Mat());
		Imgproc.cvtColor(frameInProcess, frameInProcess,
				Imgproc.COLOR_BGR2YCrCb);
		Core.inRange(frameInProcess, new Scalar(0, 131, 80), new Scalar(255,
				185, 135), frameInProcess);
		Imgproc.medianBlur(frameInProcess, frameInProcess, 5);
		// Core.bitwise_not(frameInProcess, frameInProcess);
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

		extractFeature();

		row++;
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
					data[row][index] = 0;
				else
					data[row][index] = 1;
				index++;
				subImg.release();
			}
		}
	}

	private void train() {

		CvSVMParams params = new CvSVMParams();
		params.set_svm_type(CvSVM.C_SVC);
		params.set_kernel_type(CvSVM.LINEAR);
		params.set_term_crit(new TermCriteria(TermCriteria.MAX_ITER, 100, 1e-6));

		assign_lables();

		Mat labelsMat = new Mat(dataSetSize, 1, CvType.CV_32FC1);
		labelsMat.put(0, 0, labels);

		Mat trainingMat = new Mat(dataSetSize, 100, CvType.CV_32FC1);
		for (int i = 0; i < dataSetSize; i++)
			trainingMat.put(i, 0, data[i]);

		SVM = new CvSVM();
		SVM.train(trainingMat, labelsMat, new Mat(), new Mat(), params);
		File datasetFile = new File(Environment.getExternalStorageDirectory(),
				"AndroidDataset.xml");
		SVM.save(datasetFile.getAbsolutePath());
	}

	private void assign_lables() {
		int index = 0;
		// up
		for (int i = 0; i < 14; i++) {
			labels[index] = 1;
			index++;
		}
		// open
		for (int i = 0; i < 22; i++) {
			labels[index] = 2;
			index++;
		}
		// capture
		for (int i = 0; i < 38; i++) {
			labels[index] = 3;
			index++;
		}
		// call
		for (int i = 0; i < 25; i++) {
			labels[index] = 4;
			index++;
		}
		// left
		for (int i = 0; i < 22; i++) {
			labels[index] = 5;
			index++;
		}
		// right
		for (int i = 0; i < 22; i++) {
			labels[index] = 6;
			index++;
		}
		// closed
		for (int i = 0; i < 15; i++) {
			labels[index] = 7;
			index++;
		}
		// start
		for (int i = 0; i < 18; i++) {
			labels[index] = 8;
			index++;
		}
		// false
		for (int i = 0; i < 1; i++) {
			labels[index] = 9;
			index++;
		}
	}

}
