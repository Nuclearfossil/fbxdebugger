class AnimBlock;

namespace fbxsdk
{
	class FbxImporter;
	class FbxScene;
}

AnimBlock* BuildAnimBlock(fbxsdk::FbxImporter* importer, fbxsdk::FbxScene* scene);
