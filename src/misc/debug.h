#pragma once

#include <vector>
#include <fbxsdk.h>

void ProcessAnimLayer(FbxNode* node, FbxAnimStack* animStack, int animIndex);
void ProcessAnimChildNode(FbxNode* node, FbxAnimStack* animStack, FbxAnimLayer* layer);

void DisplayCurveData(fbxsdk::FbxNode * node, fbxsdk::FbxAnimLayer * layer);