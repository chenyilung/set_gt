# set_gt

## Introduction

In this script, we decide the result is tracking(true) or loss(false) according to the predicted bbox and the label bbox.

## Prepare

Before we starting to set the ground truth, there is some work we need to do first:
- First: bbox_pred.txt is needed.
- Second: bbox_gt.txt is needed.
- Third: the image result by running with some algorithm(Help the user to judge the result).

## Control direction

If you need for a help:
 - Press 'h': Get some tips.

If you judge the result is TRUE:
 (The predicted bbox and the label bbox are overlap a lot!)
  - Press 'j': Save the true(1) result in the gt.txt with this frame.

If you judge the result is FALSE:
 (The predicted bbox and the label bbox are far away!)
  - Press 'k': Save the false(0) result in the gt.txt with this frame.

If you want to rectify the previous result:
 -  Press 'a': It will turn back to the previous frame and you can reset the result.
