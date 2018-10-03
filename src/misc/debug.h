#pragma once

#include <vector>
#include <fbxsdk.h>

void ProcessCameraAnimLayer(FbxNodeAttribute* attr, FbxAnimStack* animStack, FbxAnimLayer* layer);
void ProcessAnimLayer(FbxNode* node, FbxAnimStack* animStack, FbxAnimLayer* layer);
void ProcessAnimChildNode(FbxNode* node, FbxAnimStack* animStack, FbxAnimLayer* layer);

void DisplayCurveData(fbxsdk::FbxNode * node, fbxsdk::FbxAnimLayer * layer);