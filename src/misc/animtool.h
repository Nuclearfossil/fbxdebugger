class Anim;

namespace fbxsdk
{
	class FbxImporter;
	class FbxScene;
}

Anim* BuildAnimBlock(fbxsdk::FbxImporter* importer, fbxsdk::FbxScene* scene);
const char* GetDebugText();
