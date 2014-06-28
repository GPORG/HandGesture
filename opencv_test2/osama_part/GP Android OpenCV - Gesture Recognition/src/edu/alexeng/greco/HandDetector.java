package edu.alexeng.greco;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

import org.opencv.core.Core;
import org.opencv.core.Mat;
import org.opencv.core.MatOfInt;
import org.opencv.core.MatOfInt4;
import org.opencv.core.MatOfPoint;
import org.opencv.core.MatOfPoint2f;
import org.opencv.core.Point;
import org.opencv.core.RotatedRect;
import org.opencv.core.Scalar;
import org.opencv.core.Size;
import org.opencv.imgproc.Imgproc;

public class HandDetector {
	public void process(Mat mFrame) {
		Mat res = new Mat();
		Imgproc.GaussianBlur(mFrame, res, new Size(5, 5), 0, 0);
		Imgproc.erode(res, res, new Mat());
		Imgproc.dilate(res, res, new Mat());
		Imgproc.cvtColor(res, res, Imgproc.COLOR_BGR2YCrCb);
		Core.inRange(res, new Scalar(0, 131, 80), new Scalar(255, 185, 135),
				res);
		Imgproc.medianBlur(res, res, 5);
		Core.bitwise_not(res, res);
		// finding contours
		List<MatOfPoint> contours = new ArrayList<MatOfPoint>();
		Imgproc.findContours(res, contours, new Mat(), Imgproc.RETR_LIST,
				Imgproc.CHAIN_APPROX_SIMPLE);

		// finding largest contour
		int largestContourIndex = -1;
		MatOfPoint largestContour = null;
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

		// draw largest contour
		Imgproc.drawContours(mFrame, contours, largestContourIndex, new Scalar(
				255, 0, 0));
		MatOfInt hull = new MatOfInt();
		Imgproc.convexHull(largestContour, hull);
		// draw convex hull of the largest contour
		Point p0 = new Point(largestContour.get((int) hull.get(0, 0)[0], 0));
		for (int i = 1; i < hull.rows(); i++) {
			Point pi = new Point(largestContour.get((int) hull.get(i, 0)[0], 0));
			Core.line(mFrame, p0, pi, new Scalar(0, 255, 0));
			p0 = pi;
		}

		// Convexity defects
		MatOfInt4 defects = new MatOfInt4();
		Imgproc.convexityDefects(largestContour, hull, defects);
		RotatedRect handBoundary = Imgproc.minAreaRect(new MatOfPoint2f(
				largestContour.toArray()));
		double l;
		if (handBoundary.size.height >= handBoundary.size.width)
			l = handBoundary.size.height;
		else
			l = handBoundary.size.width;
		double thersh = l / 4;
		Point p, s, e; // holder a point in each iteration
		double l1, l2, angle, dot;
		Point center = new Point(handBoundary.center.x, handBoundary.center.y);
		int numb = 0;
		for (int i = 0; i < defects.total(); i++) {
			/*
			 * MatOfInt4 (start_index, end_index, farthest_pt_index, fixpt_depth
			 */
			double[] d = defects.get(i, 0);
			double start = d[0];
			double end = d[1];
			double dFarthestPtIndex = d[2];
			double dDepth = d[3];
			if (dDepth > 10) {
				p = new Point(largestContour.get((int) dFarthestPtIndex, 0));
				s = new Point(largestContour.get((int) start, 0));
				e = new Point(largestContour.get((int) end, 0));
				l1 = Math.sqrt(Math.pow(s.x - p.x, 2) + Math.pow(s.y - p.y, 2));
				l2 = Math.sqrt(Math.pow(e.x - p.x, 2) + Math.pow(e.y - p.y, 2));
				dot = (s.x - p.x) * (e.x - p.x) + (s.y - p.y) * (e.y - p.y);
				angle = Math.acos(dot / (l1 * l2));
				angle = angle * 180 / (3.14159);
				if (!(l1 < thersh && l2 < thersh && angle > 95)) {
					Core.circle(mFrame, p, 5, new Scalar(255, 255, 0));
					numb++;
				}
			}
		}
		Core.circle(mFrame, center, 5, new Scalar(0, 0, 255));
	}
}
