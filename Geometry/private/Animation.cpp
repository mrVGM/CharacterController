#include "Animation.h"

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
	BasicObjectContainer<geo::AnimationTypeDef> m_animationTypeDef;
}

const geo::AnimationTypeDef& geo::AnimationTypeDef::GetTypeDef()
{
	if (!m_animationTypeDef.m_object)
	{
		m_animationTypeDef.m_object = new AnimationTypeDef();
	}

	return *m_animationTypeDef.m_object;
}

geo::AnimationTypeDef::AnimationTypeDef() :
	ReferenceTypeDef(&ReferenceTypeDef::GetTypeDef(), "B2836B8D-7C9B-436F-A9DA-4A0238FCAD6B"),
	m_colladaFile("D02EEA99-AD2D-4777-A9AC-B6C1DE3C99D5", StringTypeDef::GetTypeDef()),
	m_hash("DFBBD880-6B60-4B0B-AFE0-F34AE19A7C75", StringTypeDef::GetTypeDef())
{
	{
		m_colladaFile.m_name = "Collada File";
		m_colladaFile.m_category = "Setup";
		m_colladaFile.m_getValue = [](CompositeValue* obj) -> Value& {
			Animation* animation = static_cast<Animation*>(obj);
			return animation->m_colladaFile;
		};
		m_properties[m_colladaFile.GetId()] = &m_colladaFile;
	}

	{
		m_hash.m_name = "Hash";
		m_hash.m_category = "Internal";
		m_hash.m_getValue = [](CompositeValue* obj) -> Value& {
			Animation* animation = static_cast<Animation*>(obj);
			return animation->m_hash;
		};
		m_properties[m_hash.GetId()] = &m_hash;
	}

	m_name = "Animation";
	m_category = "Animations";
}

geo::AnimationTypeDef::~AnimationTypeDef()
{
}

void geo::AnimationTypeDef::Construct(Value& container) const
{
	Animation* animation = new Animation(*this);
	container.AssignObject(animation);
}

geo::Animation::Animation(const ReferenceTypeDef& type) :
	ObjectValue(type),
	m_hash(AnimationTypeDef::GetTypeDef().m_hash.GetType(), this),
	m_colladaFile(AnimationTypeDef::GetTypeDef().m_colladaFile.GetType(), this)
{
}

geo::Animation::~Animation()
{
}

int geo::Animation::GetBoneIndex(const std::string& boneName) const
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

void geo::Animation::SerializeToMF(files::MemoryFile& mf)
{
	using namespace files;

	files::MemoryFileWriter writer(mf);
}

void geo::Animation::DeserializeFromMF(files::MemoryFile& mf)
{
	using namespace files;

	files::MemoryFileReader reader(mf);
}

void geo::Animation::LoadData(jobs::Job* done)
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

	const Node* animationsLib = tree.FindNode([](const Node* node) {
		return node->m_tagName == "library_animations";
	});

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
		defaultsMap[AnimationTypeDef::GetTypeDef().m_hash.GetId()] = json_parser::JSONValue(hash);
		asset->SaveJSONData();
	}

	jobs::RunSync(done);
}
