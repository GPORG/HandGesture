HandGesture
===========
HandGesture
===========
A simple gesture recognition system using OpenCv library.                                                                   
Its main objective is to help users to interact with their devices in more reliable way.                                    
We implemented three different approaches to extract features from hand:                                                    
1- Gridding:                                                                                                                
  i-    Get the binary representation of the hand.                                                                          
  ii-   Split it into N Grids.                                                                                              
  iii-  Each grid votes as 0 or 1.                                                                                          
  v-    collect votes and pass it to classifier.                                                                                                                                                                                                        

2- Predefined Features Extraction.                                                                                          
  The Following Features are extracted from hand:                                                                           
    i-    Mean and Variance of hand gray image.                                                                             
    ii-   Hand Area and Permiter.                                                                                           
    iii-  Hand Orientation.                                                                                                 
    v-    Orientation Histogram.                                                                                            
    iv-   Radial Signature.                                                                                                 
                                                                                                                            
3- SIFT and Bag Of Words.                                                                                                     
                                                                                                                            
Then the extracted features are trained using a proper classification technique (SVM).                                      
                                                                                                                            
A simple Demo with its description can be found at : https://www.youtube.com/watch?v=YQxCUGtSPRc                            
