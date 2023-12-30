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
	m_loader(*this),
	m_hash(SkeletonTypeDef::GetTypeDef().m_hash.GetType(), this),
	m_colladaFile(SkeletonTypeDef::GetTypeDef().m_colladaFile.GetType(), this),
	m_buffers(ReferenceTypeDef::GetTypeDef(), this)
{
}

geo::Skeleton::~Skeleton()
{
}

void geo::Skeleton::Load(jobs::Job* done)
{
	m_loader.Load(done);
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
		//DeserializeFromMF(mf);
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
		std::string id = cur->m_tagProps.find("id")->second;
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

	jobs::RunSync(done);
}
