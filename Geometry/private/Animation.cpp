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

	struct AnimationReader
	{
		const xml_reader::XMLTree& m_tree;
		const xml_reader::Node* m_animationNode = nullptr;
		std::map<std::string, std::string> m_boneIdToName;

		AnimationReader(const xml_reader::XMLTree& tree) :
			m_tree(tree)
		{
		}

		const xml_reader::Node* GetAnimationNode()
		{
			using namespace xml_reader;

			if (m_animationNode)
			{
				return m_animationNode;
			}

			const Node* animationsLib = m_tree.FindNode([](const Node* node) {
				return node->m_tagName == "library_animations";
			});

			const Node* animation = m_tree.FindChildNode(animationsLib, [](const Node* node) {
				return node->m_tagName == "animation";
			}, true);

			m_animationNode = animation;

			return m_animationNode;
		}

		void ReadJoints()
		{
			using namespace xml_reader;

			const Node* libVisualScenes = m_tree.FindNode([](const Node* node) {
				return node->m_tagName == "library_visual_scenes";
			});

			const Node* visualScene = m_tree.FindChildNode(libVisualScenes, [](const Node* node) {
				return node->m_tagName == "visual_scene";
			}, true);

			std::vector<const Node*> joints;
			{
				std::list<const Node*> tmp;
				m_tree.FindChildNodes(visualScene, [](const Node* node) {
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
				std::string name = cur->m_tagProps.find("name")->second;
				m_boneIdToName[cur->m_tagProps.find("id")->second] = name;
			}
		}

		void ReadTime(const xml_reader::Node* timeSource, std::list<float>& timeArray)
		{
			using namespace xml_reader;

			const Node* accessor = m_tree.FindChildNode(timeSource, [](const Node* node) {
				return node->m_tagName == "accessor";
			}, false);

			std::string sourceId = accessor->m_tagProps.find("source")->second.c_str() + 1;
			const Node* floatArray = m_tree.FindChildNode(timeSource, [&](const Node* node) {
				if (node->m_tagName != "float_array")
				{
					return false;
				}

				return node->m_tagProps.find("id")->second == sourceId;
			}, true);

			std::stringstream ss;
			int count;
			ss << accessor->m_tagProps.find("count")->second;
			ss >> count;

			auto floatIt = floatArray->m_data.begin();
			for (int i = 0; i < count; ++i)
			{
				timeArray.push_back((*(floatIt++))->m_symbolData.m_number);
			}
		}

		void ReadTransforms(const xml_reader::Node* transformSource, std::list<math::Matrix>& transformArray)
		{
			using namespace xml_reader;

			const Node* accessor = m_tree.FindChildNode(transformSource, [](const Node* node) {
				return node->m_tagName == "accessor";
			}, false);

			std::string sourceId = accessor->m_tagProps.find("source")->second.c_str() + 1;
			const Node* floatArray = m_tree.FindChildNode(transformSource, [&](const Node* node) {
				if (node->m_tagName != "float_array")
				{
					return false;
				}

				return node->m_tagProps.find("id")->second == sourceId;
			}, true);

			std::stringstream ss;
			int count;
			ss << accessor->m_tagProps.find("count")->second;
			ss >> count;

			auto floatIt = floatArray->m_data.begin();
			for (int i = 0; i < count; ++i)
			{
				math::Matrix& mat = transformArray.emplace_back();
				for (int j = 0; j < 16; ++j)
				{
					mat.m_coefs[j] = (*(floatIt++))->m_symbolData.m_number;
				}
			}
		}

		void ReadAnimChannel(const xml_reader::Node* animNode, geo::Animation::AnimChannel& outChannel)
		{
			using namespace xml_reader;

			const Node* channel = m_tree.FindChildNode(animNode, [](const Node* node) {
				return node->m_tagName == "channel";
			}, true);

			std::string target = channel->m_tagProps.find("target")->second;
			int end = target.find('//');
			target = target.substr(0, end);

			outChannel.m_name = m_boneIdToName[target];

			const Node* sampler = m_tree.FindChildNode(animNode, [](const Node* node) {
				return node->m_tagName == "sampler";
			}, true);

			const Node* input = m_tree.FindChildNode(sampler, [](const Node* node) {
				if (node->m_tagName != "input")
				{
					return false;
				}

				return node->m_tagProps.find("semantic")->second == "INPUT";
			}, true);

			const Node* output = m_tree.FindChildNode(sampler, [](const Node* node) {
				if (node->m_tagName != "input")
				{
					return false;
				}

				return node->m_tagProps.find("semantic")->second == "OUTPUT";
			}, true);

			std::string inputSourceId = input->m_tagProps.find("source")->second.c_str() + 1;
			std::string outputSourceId = output->m_tagProps.find("source")->second.c_str() + 1;

			const Node* inputSource = m_tree.FindChildNode(animNode, [&](const Node* node) {
				if (node->m_tagName != "source")
				{
					return false;
				}

				return node->m_tagProps.find("id")->second == inputSourceId;
			}, true);

			const Node* outputSource = m_tree.FindChildNode(animNode, [&](const Node* node) {
				if (node->m_tagName != "source")
				{
					return false;
				}

				return node->m_tagProps.find("id")->second == outputSourceId;
			}, true);

			std::list<float> timeArray;
			std::list<math::Matrix> transformArray;

			ReadTime(inputSource, timeArray);
			ReadTransforms(outputSource, transformArray);

			auto timeIt = timeArray.begin();
			auto transformIt = transformArray.begin();


			while (timeIt != timeArray.end())
			{
				geo::Animation::KeyFrame& kf = outChannel.m_keyFrames.emplace_back();
				kf.m_time = *(timeIt++);
				kf.m_transform = *(transformIt++);
			}
		}

		void ReadAnimation(std::vector<geo::Animation::AnimChannel>& animation)
		{
			using namespace xml_reader;

			GetAnimationNode();
			ReadJoints();

			std::list<const Node*> channels;

			m_tree.FindChildNodes(m_animationNode, [](const Node* node) {
				return node->m_tagName == "animation";
			}, true, channels);


			for (auto it = channels.begin(); it != channels.end(); ++it)
			{
				geo::Animation::AnimChannel& channel = animation.emplace_back();
				ReadAnimChannel(*it, channel);
			}
		}
	};

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

void geo::Animation::SerializeToMF(files::MemoryFile& mf)
{
	using namespace files;

	files::MemoryFileWriter writer(mf);

	{
		std::vector<std::string> m_boneNames;

		for (auto it = m_animation.begin(); it != m_animation.end(); ++it)
		{
			m_boneNames.push_back((*it).m_name);
		}

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

	for (auto it = m_animation.begin(); it != m_animation.end(); ++it)
	{
		BinChunk kfChunk;
		AnimChannel& cur = *it;

		kfChunk.m_size = cur.m_keyFrames.size() * sizeof(KeyFrame);
		kfChunk.m_data = new char[kfChunk.m_size];

		KeyFrame* kfPtr = reinterpret_cast<KeyFrame*>(kfChunk.m_data);
		for (int i = 0; i < cur.m_keyFrames.size(); ++i)
		{
			*(kfPtr++) = cur.m_keyFrames[i];
		}

		kfChunk.Write(writer);
	}
}

void geo::Animation::DeserializeFromMF(files::MemoryFile& mf)
{
	using namespace files;

	files::MemoryFileReader reader(mf);

	std::vector<std::string> m_boneNames;
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

	for (auto it = m_boneNames.begin(); it != m_boneNames.end(); ++it)
	{
		BinChunk kfChunk;
		kfChunk.Read(reader);

		AnimChannel& curChannel = m_animation.emplace_back();
		curChannel.m_name = *it;

		int kfCount = kfChunk.m_size / sizeof(KeyFrame);
		KeyFrame* kfPtr = reinterpret_cast<KeyFrame*>(kfChunk.m_data);
		for (int i = 0; i < kfCount; ++i)
		{
			KeyFrame& curFrame = curChannel.m_keyFrames.emplace_back();
			curFrame = *(kfPtr++);
		}
	}
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

	AnimationReader ar(tree);
	ar.ReadAnimation(m_animation);

	if (m_zUp)
	{
		for (auto it = m_animation.begin(); it != m_animation.end(); ++it)
		{
			AnimChannel& cur = *it;
			for (auto kfIt = cur.m_keyFrames.begin(); kfIt != cur.m_keyFrames.end(); ++kfIt)
			{
				KeyFrame& kf = *kfIt;
				kf.m_transform = kf.m_transform.FlipYZAxis();
			}
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
		defaultsMap[AnimationTypeDef::GetTypeDef().m_hash.GetId()] = json_parser::JSONValue(hash);
		asset->SaveJSONData();
	}

	jobs::RunSync(done);
}
