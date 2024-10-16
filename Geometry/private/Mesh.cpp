#include "Mesh.h"

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

#include "Settings.h"

#include <map>
#include <string>
#include <vector>
#include <list>
#include <sstream>

namespace
{
	BasicObjectContainer<geo::MeshTypeDef> m_meshTypeDef;

	struct MeshReader
	{
		const xml_reader::XMLTree& m_tree;
		const xml_reader::Node* m_meshNode = nullptr;

		std::vector<math::Vector3> m_positions;
		std::vector<math::Vector3> m_normals;
		std::vector<math::Vector2> m_uvs;

		std::vector<int> m_vertexToPositionMap;
		std::vector<geo::MeshVertex> m_verts;
		std::map<std::string, int> m_vertexMap;

		MeshReader(const xml_reader::XMLTree& tree) :
			m_tree(tree)
		{
		}

		bool EqualVertices(const geo::MeshVertex& v1, const geo::MeshVertex& v2)
		{
			using namespace math;
			
			{
				Vector3 tmp = v1.m_position + (-1 * v2.m_position);
				if (Dot(tmp, tmp) >= GetFloatEPS())
				{
					return false;
				}
			}

			{
				Vector3 tmp = v1.m_normal + (-1 * v2.m_normal);
				if (Dot(tmp, tmp) >= GetFloatEPS())
				{
					return false;
				}
			}

			{
				Vector2 tmp = v1.m_uv + (-1 * v2.m_uv);
				if (Dot(tmp, tmp) >= GetFloatEPS())
				{
					return false;
				}
			}

			return true;
		}

		int FindVertex(const geo::MeshVertex& vert)
		{
			for (int i = 0; i < m_verts.size(); ++i)
			{
				if (EqualVertices(vert, m_verts[i]))
				{
					return i;
				}
			}

			return -1;
		}

		const xml_reader::Node* GetMeshNode()
		{
			using namespace xml_reader;

			if (m_meshNode)
			{
				return m_meshNode;
			}

			const Node* libGeometries = m_tree.FindNode([](const Node* node) {
				return node->m_tagName == "library_geometries";
			});

			const Node* geometry = m_tree.FindChildNode(libGeometries, [](const Node* node) {
				return node->m_tagName == "geometry";
			}, true);

			const Node* mesh = m_tree.FindChildNode(geometry, [](const Node* node) {
				return node->m_tagName == "mesh";
			}, true);

			m_meshNode = mesh;
			return m_meshNode;
		}

		void GetTrianglesNodes(std::list<const xml_reader::Node*>& outTrianglesNodes)
		{
			using namespace xml_reader;

			m_tree.FindChildNodes(GetMeshNode(), [](const Node* node) {
				return node->m_tagName == "triangles";
			}, true, outTrianglesNodes);
		}

		void ReadVector3s(const xml_reader::Node* source, std::vector<math::Vector3>& vects)
		{
			if (!vects.empty())
			{
				return;
			}

			using namespace xml_reader;

			const Node* accessor = m_tree.FindChildNode(
				source,
				[](const Node* node) {
					return node->m_tagName == "accessor";
				},
				false);

			std::string accessorSourceId = accessor->m_tagProps.find("source")->second.c_str() + 1;

			const Node* floatArray = m_tree.FindChildNode(
				source,
				[&accessorSourceId](const Node* node) {
					if (node->m_tagName != "float_array")
					{
						return false;
					}

					return node->m_tagProps.find("id")->second == accessorSourceId;
				},
				true);

			int xIndex = -1;
			int yIndex = -1;
			int zIndex = -1;

			int index = 0;
			for (auto it = accessor->m_children.begin(); it != accessor->m_children.end(); ++it)
			{
				const Node* cur = *it;
				if (cur->m_tagName != "param")
				{
					continue;
				}
				int curIndex = index++;

				if (cur->m_tagProps.find("name")->second == "X")
				{
					xIndex = curIndex;
					continue;
				}

				if (cur->m_tagProps.find("name")->second == "Y")
				{
					yIndex = curIndex;
					continue;
				}

				if (cur->m_tagProps.find("name")->second == "Z")
				{
					zIndex = curIndex;
					continue;
				}
			}

			std::stringstream ss;
			ss << (accessor->m_tagProps.find("count")->second);
			int count;
			ss >> count;
			ss.clear();

			ss << (accessor->m_tagProps.find("stride")->second);
			int stride;
			ss >> stride;
			ss.clear();

			auto it = floatArray->m_data.begin();
			for (int i = 0; i < count; ++i)
			{
				math::Vector3& vect = vects.emplace_back();
				for (int j = 0; j < stride; ++j)
				{
					scripting::ISymbol* cur = *it;
					++it;
					if (j == xIndex)
					{
						vect.m_coefs[0] = cur->m_symbolData.m_number;
						continue;
					}
					if (j == yIndex)
					{
						vect.m_coefs[1] = cur->m_symbolData.m_number;
						continue;
					}
					if (j == zIndex)
					{
						vect.m_coefs[2] = cur->m_symbolData.m_number;
						continue;
					}
				}
			}
		}

		void ReadUVs(const xml_reader::Node* source, std::vector<math::Vector2>& uvs)
		{
			if (!uvs.empty())
			{
				return;
			}

			using namespace xml_reader;

			const Node* accessor = m_tree.FindChildNode(
				source,
				[](const Node* node) {
					return node->m_tagName == "accessor";
				},
				false);

			std::string accessorSourceId = accessor->m_tagProps.find("source")->second.c_str() + 1;

			const Node* floatArray = m_tree.FindChildNode(
				source,
				[&accessorSourceId](const Node* node) {
					if (node->m_tagName != "float_array")
					{
						return false;
					}

					return node->m_tagProps.find("id")->second == accessorSourceId;
				},
				true);

			int xIndex = -1;
			int yIndex = -1;

			int index = 0;
			for (auto it = accessor->m_children.begin(); it != accessor->m_children.end(); ++it)
			{
				const Node* cur = *it;
				if (cur->m_tagName != "param")
				{
					continue;
				}
				int curIndex = index++;

				if (cur->m_tagProps.find("name")->second == "S")
				{
					xIndex = curIndex;
					continue;
				}

				if (cur->m_tagProps.find("name")->second == "T")
				{
					yIndex = curIndex;
					continue;
				}
			}

			std::stringstream ss;
			ss << (accessor->m_tagProps.find("count")->second);
			int count;
			ss >> count;
			ss.clear();

			ss << (accessor->m_tagProps.find("stride")->second);
			int stride;
			ss >> stride;
			ss.clear();

			auto it = floatArray->m_data.begin();
			for (int i = 0; i < count; ++i)
			{
				math::Vector2& vect = uvs.emplace_back();
				for (int j = 0; j < stride; ++j)
				{
					scripting::ISymbol* cur = *it;
					++it;
					if (j == xIndex)
					{
						vect.m_coefs[0] = cur->m_symbolData.m_number;
						continue;
					}
					if (j == yIndex)
					{
						vect.m_coefs[1] = cur->m_symbolData.m_number;
						continue;
					}
				}
			}
		}

		void ReadTriangles(const xml_reader::Node* triangles, std::list<int>& indices)
		{
			using namespace xml_reader;
			const Node* vertexInput = m_tree.FindChildNode(
				triangles,
				[](const Node* node) {
					if (node->m_tagName != "input") {
						return false;
					}
					return node->m_tagProps.find("semantic")->second == "VERTEX";
				},
				true);

			const Node* normalInput = m_tree.FindChildNode(
				triangles,
				[](const Node* node) {
					if (node->m_tagName != "input") {
						return false;
					}
					return node->m_tagProps.find("semantic")->second == "NORMAL";
				},
				true);

			const Node* uvInput = m_tree.FindChildNode(
				triangles,
				[](const Node* node) {
					if (node->m_tagName != "input") {
						return false;
					}
					return node->m_tagProps.find("semantic")->second == "TEXCOORD";
				},
				true);

			const Node* vertexSourceNode = nullptr;
			const Node* normalSourceNode = nullptr;
			const Node* uvSourceNode = nullptr;

			{
				std::string vertsId = vertexInput->m_tagProps.find("source")->second.c_str() + 1;
				const Node* vertices = m_tree.FindChildNode(
					GetMeshNode(),
					[&vertsId](const Node* node) {
						if (node->m_tagName != "vertices")
						{
							return false;
						}

						return node->m_tagProps.find("id")->second == vertsId;
					},
					true);

				const Node* positionsInput = m_tree.FindChildNode(
					vertices,
					[](const Node* node) {
						if (node->m_tagName != "input")
						{
							return false;
						}

						return node->m_tagProps.find("semantic")->second == "POSITION";
					}, true);

				std::string vertexSourceId = positionsInput->m_tagProps.find("source")->second.c_str() + 1;

				vertexSourceNode = m_tree.FindChildNode(
					GetMeshNode(),
					[&vertexSourceId](const Node* node) {
						if (node->m_tagName != "source")
						{
							return false;
						}

						return node->m_tagProps.find("id")->second == vertexSourceId;
					},
					true);
			}

			{
				std::string normalSourceId = normalInput->m_tagProps.find("source")->second.c_str() + 1;
				normalSourceNode = m_tree.FindChildNode(
					GetMeshNode(),
					[&normalSourceId](const Node* node) {
						if (node->m_tagName != "source")
						{
							return false;
						}

						return node->m_tagProps.find("id")->second == normalSourceId;
					},
					true);

			}

			{
				std::string uvSourceId = uvInput->m_tagProps.find("source")->second.c_str() + 1;
				uvSourceNode = m_tree.FindChildNode(
					GetMeshNode(),
					[&uvSourceId](const Node* node) {
						if (node->m_tagName != "source")
						{
							return false;
						}

						return node->m_tagProps.find("id")->second == uvSourceId;
					},
					true);

			}

			ReadVector3s(vertexSourceNode, m_positions);
			ReadVector3s(normalSourceNode, m_normals);
			ReadUVs(uvSourceNode, m_uvs);

			const Node* p = m_tree.FindChildNode(
				triangles,
				[](const Node* node) {
					return node->m_tagName == "p";
				},
				true);

			int count;
			std::stringstream ss;
			ss << triangles->m_tagProps.find("count")->second;
			ss >> count;

			int stride = 0;
			{
				std::list<const Node*> inputNodes;
				m_tree.FindChildNodes(
					triangles,
					[](const Node* node) {
						return node->m_tagName == "input";
					},
					true,
					inputNodes);
				stride = inputNodes.size();
			}

			int vertexOffset = -1;
			int normalOffset = -1;
			int uvOffset = -1;

			{
				ss.clear();
				ss << vertexInput->m_tagProps.find("offset")->second;
				ss >> vertexOffset;

				ss.clear();
				ss << normalInput->m_tagProps.find("offset")->second;
				ss >> normalOffset;

				ss.clear();
				ss << uvInput->m_tagProps.find("offset")->second;
				ss >> uvOffset;
			}

			auto pIt = p->m_data.begin();
			for (int i = 0; i < count; ++i)
			{
				for (int k = 0; k < 3; ++k)
				{

					int vertIndex = -1;
					int normalIndex = -1;
					int uvIndex = -1;

					for (int j = 0; j < stride; ++j)
					{
						scripting::ISymbol* cur = *pIt;
						++pIt;

						if (j == vertexOffset)
						{
							vertIndex = static_cast<int>(cur->m_symbolData.m_number);
							continue;
						}
						if (j == normalOffset)
						{
							normalIndex = static_cast<int>(cur->m_symbolData.m_number);
							continue;
						}
						if (j == uvOffset)
						{
							uvIndex = static_cast<int>(cur->m_symbolData.m_number);
							continue;
						}
					}

					geo::MeshVertex vert
					{
						m_positions[vertIndex],
						m_normals[normalIndex],
						m_uvs[uvIndex]
					};
					int meshVertexIndex = FindVertex(vert);
					if (meshVertexIndex < 0)
					{
						m_verts.push_back(vert);
						m_vertexToPositionMap.push_back(vertIndex);

						meshVertexIndex = m_verts.size() - 1;
					}

					indices.push_back(meshVertexIndex);
				}
			}
		}

		void ReadJointNames(const xml_reader::Node* skin, geo::Mesh::SkinData& skinData)
		{
			using namespace xml_reader;

			std::map<std::string, std::string> boneIdToName;
			{
				const Node* lib = m_tree.FindNode([](const Node* node) {
					return node->m_tagName == "library_visual_scenes";
				});

				const Node* visualScene = m_tree.FindChildNode(lib, [](const Node* node) {
					return node->m_tagName == "visual_scene";
				}, true);

				std::list<const Node*> joints;
				m_tree.FindChildNodes(visualScene, [](const Node* node) {
					if (node->m_tagName != "node")
					{
						return false;
					}

					return node->m_tagProps.find("type")->second == "JOINT";
				}, false, joints);

				for (auto it = joints.begin(); it != joints.end(); ++it)
				{
					const Node* cur = *it;
					boneIdToName[cur->m_tagProps.find("sid")->second] = cur->m_tagProps.find("name")->second;
				}
			}

			const Node* joints = m_tree.FindChildNode(skin, [](const Node* node) {
				return node->m_tagName == "joints";
			}, true);

			const Node* jointNames = m_tree.FindChildNode(joints, [](const Node* node) {
				if (node->m_tagName != "input")
				{
					return false;
				}

				return node->m_tagProps.find("semantic")->second == "JOINT";
			}, true);

			std::string jointNamesSourceId = jointNames->m_tagProps.find("source")->second;
			jointNamesSourceId = jointNamesSourceId.c_str() + 1;

			const Node* jointNamesSource = m_tree.FindChildNode(skin, [&](const Node* node) {
				if (node->m_tagName != "source")
				{
					return false;
				}

				return node->m_tagProps.find("id")->second == jointNamesSourceId;
			}, true);

			const Node* accessor = m_tree.FindChildNode(jointNamesSource, [&](const Node* node) {
				return node->m_tagName == "accessor";
			}, false);

			std::string jointsArraySourceId = accessor->m_tagProps.find("source")->second;
			jointsArraySourceId = jointsArraySourceId.c_str() + 1;

			const Node* jointsArray = m_tree.FindChildNode(jointNamesSource, [&](const Node* node) {
				if (node->m_tagName != "Name_array")
				{
					return false;
				}

				return node->m_tagProps.find("id")->second == jointsArraySourceId;
			}, true);

			std::stringstream ss;
			int jointsCount;
			ss << accessor->m_tagProps.find("count")->second;
			ss >> jointsCount;

			auto jointIt = jointsArray->m_data.begin();
			for (int i = 0; i < jointsCount; ++i)
			{
				scripting::ISymbol* cur = *(jointIt++);
				skinData.m_boneNames.push_back(boneIdToName[cur->m_symbolData.m_string]);
			}
		}

		void ReadInvBindMatrices(const xml_reader::Node* skin, geo::Mesh::SkinData& skinData)
		{
			using namespace xml_reader;

			const Node* bindShapeMatrix = m_tree.FindChildNode(skin, [](const Node* node) {
				return node->m_tagName == "bind_shape_matrix";
			}, true);

			{
				auto numIt = bindShapeMatrix->m_data.begin();
				for (int i = 0; i < 16; ++i)
				{
					scripting::ISymbol* cur = *(numIt++);
					skinData.m_bindShapeMatrix.m_coefs[i] = cur->m_symbolData.m_number;
				}
			}

			const Node* joints = m_tree.FindChildNode(skin, [](const Node* node) {
				return node->m_tagName == "joints";
			}, true);

			const Node* invBindMatricesInput = m_tree.FindChildNode(joints, [](const Node* node) {
				if (node->m_tagName != "input")
				{
					return false;
				}

				return node->m_tagProps.find("semantic")->second == "INV_BIND_MATRIX";
			}, true);

			std::string invBindMatrixSourceId = invBindMatricesInput->m_tagProps.find("source")->second;
			invBindMatrixSourceId = invBindMatrixSourceId.c_str() + 1;

			const Node* invBindMatrixSource = m_tree.FindChildNode(skin, [&](const Node* node) {
				if (node->m_tagName != "source")
				{
					return false;
				}

				return node->m_tagProps.find("id")->second == invBindMatrixSourceId;
			}, true);

			const Node* accessor = m_tree.FindChildNode(invBindMatrixSource, [&](const Node* node) {
				return node->m_tagName == "accessor";
			}, false);

			std::string floatArraySource = accessor->m_tagProps.find("source")->second;
			floatArraySource = floatArraySource.c_str() + 1;

			const Node* matrixArray = m_tree.FindChildNode(invBindMatrixSource, [&](const Node* node) {
				if (node->m_tagName != "float_array")
				{
					return false;
				}
				
				return node->m_tagProps.find("id")->second == floatArraySource;
			}, true);

			std::stringstream ss;
			int matrixCount;
			ss << accessor->m_tagProps.find("count")->second;
			ss >> matrixCount;

			auto numIt = matrixArray->m_data.begin();
			for (int i = 0; i < matrixCount; ++i)
			{
				math::Matrix& mat = skinData.m_invBindMatrices.emplace_back();
				for (int j = 0; j < 16; ++j)
				{
					mat.m_coefs[j] = (*(numIt++))->m_symbolData.m_number;
				}
			}
		}

		void ReadWeightsArray(const xml_reader::Node* skin, std::vector<float>& outWeightsArray)
		{
			using namespace xml_reader;

			const Node* weightsNode = m_tree.FindChildNode(skin, [](const Node* node) {
				return node->m_tagName == "vertex_weights";
			}, true);

			const Node* weightsArrayInput = m_tree.FindChildNode(weightsNode, [](const Node* node) {
				if (node->m_tagName != "input")
				{
					return false;
				}

				return node->m_tagProps.find("semantic")->second == "WEIGHT";
			}, true);

			std::string weightsArraySourceId = weightsArrayInput->m_tagProps.find("source")->second;
			weightsArraySourceId = weightsArraySourceId.c_str() + 1;

			const Node* weightsArraySource = m_tree.FindChildNode(skin, [&](const Node* node) {
				if (node->m_tagName != "source")
				{
					return false;
				}

				return node->m_tagProps.find("id")->second == weightsArraySourceId;
			}, true);

			const Node* accessor = m_tree.FindChildNode(weightsArraySource, [&](const Node* node) {
				return node->m_tagName == "accessor";
			}, false);

			std::string floatArraySource = accessor->m_tagProps.find("source")->second;
			floatArraySource = floatArraySource.c_str() + 1;

			const Node* weightsArray = m_tree.FindChildNode(weightsArraySource, [&](const Node* node) {
				if (node->m_tagName != "float_array")
				{
					return false;
				}

				return node->m_tagProps.find("id")->second == floatArraySource;
			}, true);

			std::stringstream ss;
			int count;
			ss << accessor->m_tagProps.find("count")->second;
			ss >> count;

			auto numIt = weightsArray->m_data.begin();
			for (int i = 0; i < count; ++i)
			{
				float cur = (*(numIt++))->m_symbolData.m_number;
				outWeightsArray.push_back(cur);
			}
		}

		void ReadWeights(const xml_reader::Node* skin, geo::Mesh::SkinData& skinData)
		{
			using namespace xml_reader;

			std::vector<float> weightsArray;
			ReadWeightsArray(skin, weightsArray);

			const Node* weightsNode = m_tree.FindChildNode(skin, [](const Node* node) {
				return node->m_tagName == "vertex_weights";
			}, true);

			int count;
			int stride = -1;

			std::stringstream ss;
			ss << weightsNode->m_tagProps.find("count")->second;
			ss >> count;

			{
				std::list<const Node*> tmp;
				m_tree.FindChildNodes(weightsNode, [](const Node* node) {
					return node->m_tagName == "input";
				}, true, tmp);

				stride = tmp.size();
			}

			int jointOffset = -1;
			int weightOffset = -1;

			{
				const Node* jointInput = m_tree.FindChildNode(weightsNode, [](const Node* node) {
					if (node->m_tagName != "input")
					{
						return false;
					}

					return node->m_tagProps.find("semantic")->second == "JOINT";
				}, true);

				ss.clear();
				ss << jointInput->m_tagProps.find("offset")->second;
				ss >> jointOffset;
			}

			{
				const Node* weightInput = m_tree.FindChildNode(weightsNode, [](const Node* node) {
					if (node->m_tagName != "input")
					{
						return false;
					}

					return node->m_tagProps.find("semantic")->second == "WEIGHT";
				}, true);

				ss.clear();
				ss << weightInput->m_tagProps.find("offset")->second;
				ss >> weightOffset;
			}

			const Node* vcount = m_tree.FindChildNode(weightsNode, [](const Node* node) {
				return node->m_tagName == "vcount";
			}, true);

			const Node* v = m_tree.FindChildNode(weightsNode, [](const Node* node) {
				return node->m_tagName == "v";
			}, true);

			auto vcountIt = vcount->m_data.begin();
			auto vIt = v->m_data.begin();

			for (int i = 0; i < count; ++i)
			{
				int curV = static_cast<int>((*(vcountIt++))->m_symbolData.m_number);

				geo::Mesh::SkinData::VertexWeights& weights = skinData.m_vertexWeights.emplace_back();

				for (int j = 0; j < curV; ++j)
				{
					for (int k = 0; k < stride; ++k)
					{
						scripting::ISymbol* curSymbol = *vIt;
						++vIt;

						if (k == jointOffset)
						{
							weights.m_jointIndex[j] = static_cast<int>(curSymbol->m_symbolData.m_number);
							continue;
						}

						if (k == weightOffset)
						{
							weights.m_jointWeight[j] = weightsArray[static_cast<int>(curSymbol->m_symbolData.m_number)];
							continue;
						}
					}
				}
			}
		}

		bool ReadSkin(geo::Mesh::SkinData& skinData, bool zUp)
		{
			using namespace xml_reader;

			const Node* libControllers = m_tree.FindNode([](const Node* node) {
				return node->m_tagName == "library_controllers";
				});

			if (!libControllers)
			{
				return false;
			}

			const Node* controller = m_tree.FindChildNode(libControllers, [](const Node* node) {
				return node->m_tagName == "controller";
				}, true);

			if (!controller)
			{
				return false;
			}

			const Node* skin = m_tree.FindChildNode(controller, [](const Node* node) {
				return node->m_tagName == "skin";
				}, true);

			if (!skin)
			{
				return false;
			}

			ReadJointNames(skin, skinData);
			ReadInvBindMatrices(skin, skinData);
			ReadWeights(skin, skinData);
			skinData.m_vertexToWeightsMap = m_vertexToPositionMap;

			if (zUp)
			{
				skinData.m_bindShapeMatrix = skinData.m_bindShapeMatrix.FlipYZAxis();

				for (auto it = skinData.m_invBindMatrices.begin(); it != skinData.m_invBindMatrices.end(); ++it)
				{
					math::Matrix& cur = *it;
					cur = cur.FlipYZAxis();
				}
			}

			skinData.m_hasAnyData = true;

			return true;
		}
	};
}

geo::MeshTypeDef::MeshTypeDef() :
	ReferenceTypeDef(&ReferenceTypeDef::GetTypeDef(), "D963BDD2-9E53-466B-BCAF-F3FB50434050"),
	m_colladaFile("D9F8FC86-02D0-4EAF-A73E-8616AB554825", StringTypeDef::GetTypeDef()),
	m_hash("11D91937-B683-4FC8-B175-37A3670916F2", StringTypeDef::GetTypeDef())
{
	{
		m_colladaFile.m_name = "Collada File";
		m_colladaFile.m_category = "Setup";
		m_colladaFile.m_getValue = [](CompositeValue* obj) -> Value& {
			Mesh* mesh = static_cast<Mesh*>(obj);
			return mesh->m_colladaFile;
		};
		m_properties[m_colladaFile.GetId()] = &m_colladaFile;
	}

	{
		m_hash.m_name = "Hash";
		m_hash.m_category = "Internal";
		m_hash.m_getValue = [](CompositeValue* obj) -> Value& {
			Mesh* mesh = static_cast<Mesh*>(obj);
			return mesh->m_hash;
		};
		m_properties[m_hash.GetId()] = &m_hash;
	}

	m_name = "Mesh";
	m_category = "Geometry";
}

geo::MeshTypeDef::~MeshTypeDef()
{
}

void geo::MeshTypeDef::Construct(Value& container) const
{
	Mesh* mesh = new Mesh(*this);
	container.AssignObject(mesh);
}

const geo::MeshTypeDef& geo::MeshTypeDef::GetTypeDef()
{
	if (!m_meshTypeDef.m_object)
	{
		m_meshTypeDef.m_object = new MeshTypeDef();
	}

	return *m_meshTypeDef.m_object;
}

geo::Mesh::Mesh(const ReferenceTypeDef& type) :
	ObjectValue(type),
	m_hash(MeshTypeDef::GetTypeDef().m_hash.GetType(), this),
	m_colladaFile(MeshTypeDef::GetTypeDef().m_colladaFile.GetType(), this),
	m_buffers(ReferenceTypeDef::GetTypeDef(), this)
{
}

geo::Mesh::~Mesh()
{
}

void geo::Mesh::LoadData(jobs::Job done)
{
	using namespace xml_reader;

	std::string colladaFile = "Geometry\\" + m_colladaFile.Get<std::string>();

	std::string contents;
	files::ReadTextFile(colladaFile, contents);

	std::string hash = crypto::HashString(colladaFile + contents);
	std::string savedHash = m_hash.Get<std::string>();

	if (settings::UseBinFiles() && hash == savedHash)
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

	const Node* libGeometries = tree.FindNode([](const Node* node) {
		return node->m_tagName == "library_geometries";
	});

	const Node* geometry = tree.FindChildNode(libGeometries, [](const Node* node) {
		return node->m_tagName == "geometry";
	}, true);

	const Node* mesh = tree.FindChildNode(geometry, [](const Node* node) {
		return node->m_tagName == "mesh";
	}, true);

	std::list<const Node*> triangles;
	tree.FindChildNodes(mesh, [](const Node* node) {
		return node->m_tagName == "triangles";
	}, true, triangles);

	int numIndices = 0;
	std::list<std::list<int>> indices;
	MeshReader mr(tree);
	for (auto it = triangles.begin(); it != triangles.end(); ++it)
	{
		std::list<int>& curIndices = indices.emplace_back();
		mr.ReadTriangles(*it, curIndices);
		MaterialRange& range = m_materials.emplace_back();
		range.m_start = numIndices;
		range.m_count = curIndices.size();
		numIndices += curIndices.size();
	}

	m_vertices = std::vector<MeshVertex>(mr.m_verts.begin(), mr.m_verts.end());
	{
		int index = 0;
		for (auto it = m_vertices.begin(); it != m_vertices.end(); ++it)
		{
			MeshVertex& cur = *it;
			cur = *it;

			if (m_zUp)
			{
				float tmp = cur.m_position.m_coefs[1];
				cur.m_position.m_coefs[1] = cur.m_position.m_coefs[2];
				cur.m_position.m_coefs[2] = tmp;

				tmp = cur.m_normal.m_coefs[1];
				cur.m_normal.m_coefs[1] = cur.m_normal.m_coefs[2];
				cur.m_normal.m_coefs[2] = tmp;
			}
		}
	}

	{
		for (auto it = indices.begin(); it != indices.end(); ++it)
		{
			const std::list<int>& cur = *it;
			for (auto curIt = cur.begin(); curIt != cur.end(); ++curIt)
			{
				m_indices.push_back(*curIt);
			}
		}

		if (m_zUp)
		{
			for (int i = 0; i < m_indices.size(); i += 3)
			{
				int tmp = m_indices[i + 1];
				m_indices[i + 1] = m_indices[i + 2];
				m_indices[i + 2] = tmp;
			}
		}
	}

	mr.ReadSkin(m_skinData, m_zUp);

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
		defaultsMap[MeshTypeDef::GetTypeDef().m_hash.GetId()] = json_parser::JSONValue(hash);
		asset->SaveJSONData();
	}

	jobs::RunSync(done);
}

void geo::Mesh::SerializeToMF(files::MemoryFile& mf)
{
	using namespace files;
	
	MemoryFileWriter writer(mf);

	{
		BinChunk vertexChunk;
		vertexChunk.m_data = reinterpret_cast<char*>(new MeshVertex[m_vertices.size()]);
		vertexChunk.m_size = m_vertices.size() * sizeof(MeshVertex);

		MeshVertex* vertsPtr = reinterpret_cast<MeshVertex*>(vertexChunk.m_data);
		for (auto it = m_vertices.begin(); it != m_vertices.end(); ++it)
		{
			*(vertsPtr++) = *it;
		}

		vertexChunk.Write(writer);
	}

	{
		BinChunk indexChunk;
		indexChunk.m_data = reinterpret_cast<char*>(new int[m_indices.size()]);
		indexChunk.m_size = m_indices.size() * sizeof(int);
		
		int* indexPtr = reinterpret_cast<int*>(indexChunk.m_data);
		for (auto it = m_indices.begin(); it != m_indices.end(); ++it)
		{
			*(indexPtr++) = *it;
		}

		indexChunk.Write(writer);
	}

	{
		BinChunk matRangesChunk;
		matRangesChunk.m_data = reinterpret_cast<char*>(new MaterialRange[m_materials.size()]);
		matRangesChunk.m_size = m_materials.size() * sizeof(MaterialRange);

		geo::Mesh::MaterialRange* cur = reinterpret_cast<MaterialRange*>(matRangesChunk.m_data);
		for (auto it = m_materials.begin(); it != m_materials.end(); ++it)
		{
			*cur = *it;
			cur += 1;
		}

		matRangesChunk.Write(writer);
	}

	m_skinData.WriteToMF(writer);
}

void geo::Mesh::DeserializeFromMF(files::MemoryFile& mf)
{
	using namespace files;

	MemoryFileReader reader(mf);

	{
		BinChunk vertexChunk;
		vertexChunk.Read(reader);

		int numVertices = vertexChunk.m_size / sizeof(MeshVertex);
		MeshVertex* vertsPtr = reinterpret_cast<MeshVertex*>(vertexChunk.m_data);
		for (int i = 0; i < numVertices; ++i)
		{
			m_vertices.push_back(vertsPtr[i]);
		}
	}

	{
		BinChunk indexChunk;
		indexChunk.Read(reader);

		int numIndices = indexChunk.m_size / sizeof(int);
		int* indexPtr = reinterpret_cast<int*>(indexChunk.m_data);
		for (int i = 0; i < numIndices; ++i)
		{
			m_indices.push_back(indexPtr[i]);
		}
	}

	{
		BinChunk matRangeChunk;
		matRangeChunk.Read(reader);

		int count = matRangeChunk.m_size / sizeof(MaterialRange);
		MaterialRange* arr = reinterpret_cast<MaterialRange*>(matRangeChunk.m_data);
		for (int i = 0; i < count; ++i)
		{
			m_materials.push_back(arr[i]);
		}
	}

	m_skinData.ReadFromMF(reader);
}

void geo::Mesh::SkinData::WriteToMF(files::MemoryFileWriter& writer)
{
	using namespace files;
	{
		BinChunk hasDataChunk;
		hasDataChunk.m_size = sizeof(bool);
		bool* hasAnyData = new bool[1];
		hasDataChunk.m_data = reinterpret_cast<char*>(hasAnyData);

		*hasAnyData = m_hasAnyData;

		hasDataChunk.Write(writer);
	}

	if (!m_hasAnyData)
	{
		return;
	}

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
		BinChunk matrixChunk;
		matrixChunk.m_size = (m_invBindMatrices.size() + 1) * sizeof(math::Matrix);
		matrixChunk.m_data = new char[matrixChunk.m_size];

		math::Matrix* matrixPtr = reinterpret_cast<math::Matrix*>(matrixChunk.m_data);

		*(matrixPtr++) = m_bindShapeMatrix;
		for (auto it = m_invBindMatrices.begin(); it != m_invBindMatrices.end(); ++it)
		{
			const math::Matrix& cur = *it;
			*(matrixPtr++) = cur;
		}
		matrixChunk.Write(writer);
	}

	{
		BinChunk weightsChunk;
		weightsChunk.m_size = (m_vertexWeights.size()) * sizeof(geo::Mesh::SkinData::VertexWeights);
		weightsChunk.m_data = new char[weightsChunk.m_size];

		geo::Mesh::SkinData::VertexWeights* weightsPtr = reinterpret_cast<geo::Mesh::SkinData::VertexWeights*>(weightsChunk.m_data);

		for (auto it = m_vertexWeights.begin(); it != m_vertexWeights.end(); ++it)
		{
			const geo::Mesh::SkinData::VertexWeights& cur = *it;
			*(weightsPtr++) = cur;
		}
		weightsChunk.Write(writer);
	}

	{
		BinChunk vertexToWeightsChunk;
		vertexToWeightsChunk.m_size = (m_vertexToWeightsMap.size()) * sizeof(int);
		vertexToWeightsChunk.m_data = new char[vertexToWeightsChunk.m_size];

		int* vertexToWeightsPtr = reinterpret_cast<int*>(vertexToWeightsChunk.m_data);
		for (auto it = m_vertexToWeightsMap.begin(); it != m_vertexToWeightsMap.end(); ++it)
		{
			*(vertexToWeightsPtr++) = *it;
		}
		vertexToWeightsChunk.Write(writer);
	}
}

void geo::Mesh::SkinData::ReadFromMF(files::MemoryFileReader& reader)
{
	using namespace files;
	{
		BinChunk hasDataChunk;
		hasDataChunk.Read(reader);

		bool* hasAnyDataPtr = reinterpret_cast<bool*>(hasDataChunk.m_data);
		m_hasAnyData = *hasAnyDataPtr;
	}

	if (!m_hasAnyData)
	{
		return;
	}

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
		BinChunk matrixChunk;
		matrixChunk.Read(reader);
		int numMatrices = matrixChunk.m_size / sizeof(math::Matrix);

		math::Matrix* matrixPtr = reinterpret_cast<math::Matrix*>(matrixChunk.m_data);
		for (int i = 0; i < numMatrices; ++i)
		{
			math::Matrix* cur = &m_bindShapeMatrix;

			if (i > 0)
			{
				math::Matrix& tmp = m_invBindMatrices.emplace_back();
				cur = &tmp;
			}
			*cur = *(matrixPtr++);
		}
	}

	{
		BinChunk weightsChunk;
		weightsChunk.Read(reader);
		int numWeights = weightsChunk.m_size / sizeof(geo::Mesh::SkinData::VertexWeights);

		geo::Mesh::SkinData::VertexWeights* weightsPtr = reinterpret_cast<geo::Mesh::SkinData::VertexWeights*>(weightsChunk.m_data);

		for (int i = 0; i < numWeights; ++i)
		{
			geo::Mesh::SkinData::VertexWeights& cur = m_vertexWeights.emplace_back();
			cur = *(weightsPtr++);
		}
	}

	{
		BinChunk vertexToWeightsChunk;
		vertexToWeightsChunk.Read(reader);
		int numIndices = vertexToWeightsChunk.m_size / sizeof(int);
		int* indexPtr = reinterpret_cast<int*>(vertexToWeightsChunk.m_data);

		for (int i = 0; i < numIndices; ++i)
		{
			int& cur = m_vertexToWeightsMap.emplace_back();
			cur = *(indexPtr++);
		}
	}
}


void geo::Mesh::InitVertexBuffer(void* dataPtr) const
{
	MeshVertex* vertPtr = reinterpret_cast<MeshVertex*>(dataPtr);
	for (auto it = m_vertices.begin(); it != m_vertices.end(); ++it)
	{
		*(vertPtr++) = *it;
	}
}

void geo::Mesh::InitIndexBuffer(void* dataPtr) const
{
	int* indexPtr = reinterpret_cast<int*>(dataPtr);
	for (auto it = m_indices.begin(); it != m_indices.end(); ++it)
	{
		*(indexPtr++) = *it;
	}
}