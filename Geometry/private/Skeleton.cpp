#include "Skeleton.h"

#include "PrimitiveTypes.h"

#include "Jobs.h"

#include "MemoryFile.h"
#include "Files.h"
#include "XMLReader.h"
#include "Hash.h"

#include "MathUtils.h"
#include "Geometry.h"

#include "JSONValue.h"

#include "AssetTypeDef.h"

#include <map>
#include <string>
#include <vector>
#include <list>
#include <sstream>

namespace
{
	BasicObjectContainer<geo::SkeletonTypeDef> m_skeletonTypeDef;
}

const geo::SkeletonTypeDef& geo::SkeletonTypeDef::GetTypeDef()
{
	if (!m_skeletonTypeDef.m_object)
	{
		m_skeletonTypeDef.m_object = new SkeletonTypeDef();
	}

	return *m_skeletonTypeDef.m_object;
}

geo::SkeletonTypeDef::SkeletonTypeDef() :
	ReferenceTypeDef(&ReferenceTypeDef::GetTypeDef(), "0625B5E5-7D98-42AE-97F0-E5332673B574"),
	m_colladaFile("1FE2522B-7B59-4AB8-9D5F-CA5DCA1A58E6", StringTypeDef::GetTypeDef()),
	m_hash("290FE9A8-4791-4FA6-A363-6CF8CAA0EAC7", StringTypeDef::GetTypeDef())
{
	{
		m_colladaFile.m_name = "Collada File";
		m_colladaFile.m_category = "Setup";
		m_colladaFile.m_getValue = [](CompositeValue* obj) -> Value& {
			Skeleton* skeleton = static_cast<Skeleton*>(obj);
			return skeleton->m_colladaFile;
		};
		m_properties[m_colladaFile.GetId()] = &m_colladaFile;
	}

	{
		m_hash.m_name = "Hash";
		m_hash.m_category = "Internal";
		m_hash.m_getValue = [](CompositeValue* obj) -> Value& {
			Skeleton* mesh = static_cast<Skeleton*>(obj);
			return mesh->m_hash;
		};
		m_properties[m_hash.GetId()] = &m_hash;
	}

	m_name = "Skeleton";
	m_category = "Geometry";
}

geo::SkeletonTypeDef::~SkeletonTypeDef()
{
}

void geo::SkeletonTypeDef::Construct(Value& container) const
{
	Skeleton* skeleton = new Skeleton(*this);
	container.AssignObject(skeleton);
}



geo::Skeleton::Skeleton(const ReferenceTypeDef& type) :
	ObjectValue(type),
	m_hash(SkeletonTypeDef::GetTypeDef().m_hash.GetType(), this),
	m_colladaFile(SkeletonTypeDef::GetTypeDef().m_colladaFile.GetType(), this),
	m_buffers(ReferenceTypeDef::GetTypeDef(), this)
{
}

geo::Skeleton::~Skeleton()
{
}

int geo::Skeleton::GetBoneIndex(const std::string& boneName) const
{
	if (m_boneIndices.empty())
	{
		std::map<std::string, int>& map = const_cast<std::map<std::string, int>&>(m_boneIndices);
		int index = 0;
		for (auto it = m_boneNames.begin(); it != m_boneNames.end(); ++it)
		{
			map[*it] = index++;
		}
	}

	return m_boneIndices.find(boneName)->second;
}

void geo::Skeleton::SerializeToMF(files::MemoryFile& mf)
{
	using namespace files;

	files::MemoryFileWriter writer(mf);
	{
		BinChunk namesChunk;

		int size = sizeof(int);
		for (auto it = m_boneNames.begin(); it != m_boneNames.end(); ++it)
		{
			size += sizeof(int) + (*it).size();
		}

		namesChunk.m_size = size * sizeof(char);
		namesChunk.m_data = new char[namesChunk.m_size];

		int* sizePtr = reinterpret_cast<int*>(namesChunk.m_data);
		*(sizePtr++) = m_boneNames.size();

		for (auto it = m_boneNames.begin(); it != m_boneNames.end(); ++it)
		{
			*(sizePtr++) = (*it).size();
		}

		char* curPos = reinterpret_cast<char*>(sizePtr);
		for (auto it = m_boneNames.begin(); it != m_boneNames.end(); ++it)
		{
			const std::string& cur = *it;
			int curSize = cur.size();
			int copySize = curSize * sizeof(char);

			memcpy(curPos, cur.c_str(), copySize);
			curPos += curSize;
		}
		namesChunk.Write(writer);
	}

	{
		BinChunk boneParentsChunk;
		boneParentsChunk.m_size = m_boneParents.size() * sizeof(int);
		boneParentsChunk.m_data = new char[boneParentsChunk.m_size];

		int* boneParentsPtr = reinterpret_cast<int*>(boneParentsChunk.m_data);

		for (auto it = m_boneParents.begin(); it != m_boneParents.end(); ++it)
		{
			int cur = *it;
			*(boneParentsPtr++) = cur;
		}
		boneParentsChunk.Write(writer);
	}

	{
		BinChunk bindPoseChunk;
		bindPoseChunk.m_size = (m_bindPose.size()) * sizeof(math::Matrix);
		bindPoseChunk.m_data = new char[bindPoseChunk.m_size];

		math::Matrix* matrixPtr = reinterpret_cast<math::Matrix*>(bindPoseChunk.m_data);

		for (auto it = m_bindPose.begin(); it != m_bindPose.end(); ++it)
		{
			const math::Matrix& cur = *it;
			*(matrixPtr++) = cur;
		}
		bindPoseChunk.Write(writer);
	}
}

void geo::Skeleton::DeserializeFromMF(files::MemoryFile& mf)
{
	using namespace files;

	files::MemoryFileReader reader(mf);

	{
		BinChunk namesChunk;
		namesChunk.Read(reader);
		int* sizes = reinterpret_cast<int*>(namesChunk.m_data);

		int count = *(sizes++);
		char* names = reinterpret_cast<char*>(sizes + count);

		for (int i = 0; i < count; ++i)
		{
			int curSize = sizes[i];
			char* tmp = new char[curSize + 1];

			memset(tmp, 0, curSize + 1);
			memcpy(tmp, names, curSize);
			names += curSize;

			m_boneNames.push_back(tmp);
			delete[] tmp;
		}
	}

	{
		BinChunk boneParentsChunk;
		boneParentsChunk.Read(reader);
		int count = boneParentsChunk.m_size / sizeof(int);

		int* parentsPtr = reinterpret_cast<int*>(boneParentsChunk.m_data);
		for (int i = 0; i < count; ++i)
		{
			m_boneParents.push_back(parentsPtr[i]);
		}
	}

	{
		BinChunk bindPoseChunk;
		bindPoseChunk.Read(reader);

		int count = bindPoseChunk.m_size / sizeof(math::Matrix);
		math::Matrix* matrixPtr = reinterpret_cast<math::Matrix*>(bindPoseChunk.m_data);

		for (int i = 0; i < count; ++i)
		{
			m_bindPose.push_back(matrixPtr[i]);
		}
	}
}

void geo::Skeleton::LoadData(jobs::Job* done)
{
	using namespace xml_reader;

	std::string colladaFile = "Geometry\\" + m_colladaFile.Get<std::string>();

	std::string contents;
	files::ReadTextFile(colladaFile, contents);

	std::string hash = crypto::HashString(colladaFile + contents);
	std::string savedHash = m_hash.Get<std::string>();

	if (hash == savedHash)
	{
		files::MemoryFile mf;
		std::string binFilePath = files::GetDataDir() + files::GetAssetsBinDir() + GetTypeDef().GetId() + ".bin";
		mf.RestoreFromFile(binFilePath);
		DeserializeFromMF(mf);
		jobs::RunSync(done);
		return;
	}

	xml_reader::XMLTree tree;
	xml_reader::ReadXML(contents, tree);

	const Node* upAxis = tree.FindNode([](const Node* node) {
		return node->m_tagName == "up_axis";
	});

	m_zUp = upAxis->m_data.front()->m_symbolData.m_string == "Z_UP";

	const Node* libVisualScenes = tree.FindNode([](const Node* node) {
		return node->m_tagName == "library_visual_scenes";
	});

	const Node* visualScene = tree.FindChildNode(libVisualScenes, [](const Node* node) {
		return node->m_tagName == "visual_scene";
	}, true);

	std::vector<const Node*> joints;
	{
		std::list<const Node*> tmp;
		tree.FindChildNodes(visualScene, [](const Node* node) {
			if (node->m_tagName != "node")
			{
				return false;
			}

			return node->m_tagProps.find("type")->second == "JOINT";
		}, false, tmp);
	
		joints = std::vector<const Node*>(tmp.begin(), tmp.end());
	}

	for (int i = 0; i < joints.size(); ++i)
	{
		const Node* cur = joints[i];
		std::string id = cur->m_tagProps.find("name")->second;
		m_boneNames.push_back(id);
	}

	for (int i = 0; i < joints.size(); ++i)
	{
		m_boneParents.push_back(-1);
		int& parentIndex = m_boneParents.back();

		const Node* cur = joints[i];
		const Node* parent = cur->m_parent;

		for (int j = 0; j < joints.size(); ++j)
		{
			if (parent == joints[j])
			{
				parentIndex = j;
				break;
			}
		}
	}

	for (auto it = joints.begin(); it != joints.end(); ++it)
	{
		const Node* matrix = tree.FindChildNode(*it, [](const Node* node) {
			return node->m_tagName == "matrix";
		}, true);

		math::Matrix& cur = m_bindPose.emplace_back();
		auto symbolIt = matrix->m_data.begin();
		for (int i = 0; i < 16; ++i)
		{
			cur.m_coefs[i] = (*(symbolIt++))->m_symbolData.m_number;
		}

		cur = cur.Transpose();
	}

	if (m_zUp)
	{
		for (auto it = m_bindPose.begin(); it != m_bindPose.end(); ++it)
		{
			math::Matrix& cur = *it;
			cur = cur.FlipYZAxis();
		}
	}

	files::MemoryFile mf;
	SerializeToMF(mf);

	std::string id = GetTypeDef().GetId();
	mf.SaveToFile(files::GetDataDir() + files::GetAssetsBinDir() + id + ".bin");

	{
		const AssetTypeDef* asset = static_cast<const AssetTypeDef*>(&GetTypeDef());
		json_parser::JSONValue& data = const_cast<json_parser::JSONValue&>(asset->GetJSONData());

		auto& map = data.GetAsObj();
		json_parser::JSONValue& defaults = map["defaults"];
		auto& defaultsMap = defaults.GetAsObj();
		defaultsMap[SkeletonTypeDef::GetTypeDef().m_hash.GetId()] = json_parser::JSONValue(hash);
		asset->SaveJSONData();
	}

	jobs::RunSync(done);
}
