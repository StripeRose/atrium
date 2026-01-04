// Filter "Graphics"

#include "Atrium_MeshPrimitives.hpp"

#include <vector>

namespace Atrium
{
	namespace MakeMeshPrimitives
	{
		MeshPrimitive::Vertex LerpVertex(const MeshPrimitive::Vertex& aVertex, const MeshPrimitive::Vertex& anOtherVertex, const float anAmount)
		{
			MeshPrimitive::Vertex vertex;
			vertex.Position = Vector3<float>::Lerp(aVertex.Position, anOtherVertex.Position, anAmount);
			vertex.Normal = Vector3<float>::Slerp(aVertex.Normal, anOtherVertex.Normal, anAmount);
			vertex.UV = Vector2<float>::Lerp(aVertex.UV, anOtherVertex.UV, anAmount);

			vertex.Binormal = Vector3<float>::Slerp(aVertex.Binormal, anOtherVertex.Binormal, anAmount);
			vertex.Tangent = Vector3<float>::Slerp(aVertex.Tangent, anOtherVertex.Tangent, anAmount);

			return vertex;
		}

		struct GeneratedTriangle
		{
			GeneratedTriangle(Vector3<float> aPosA, Vector3<float> aPosB, Vector3<float> aPosC, Vector2<float> aUVA, Vector2<float> aUVB, Vector2<float> aUVC)
			{
				Position[0] = aPosA;
				Position[1] = aPosB;
				Position[2] = aPosC;
				UV[0] = aUVA;
				UV[1] = aUVB;
				UV[2] = aUVC;
			}
			Vector3<float> Position[3];
			Vector2<float> UV[3];
		};

		void PopulateQuad(MeshPrimitive& aPrimitive);

		void AddTriangle(const MeshPrimitive::Vertex aTriangle[3], bool mergeVertices, MeshPrimitive& aPrimitive)
		{
			auto findOrAddVertex = [&aPrimitive, mergeVertices](const MeshPrimitive::Vertex& aVertex) -> std::uint32_t {
				std::size_t vertexIndex = aPrimitive.Vertices.size();
				if (mergeVertices)
				{
					for (std::size_t i = 0; i < aPrimitive.Vertices.size(); ++i)
					{
						const MeshPrimitive::Vertex& v = aPrimitive.Vertices[i];
						if (v.Position == aVertex.Position && v.UV == aVertex.UV && v.Normal == aVertex.Normal)
							vertexIndex = i;
					}
				}

				if (vertexIndex == aPrimitive.Vertices.size())
					aPrimitive.Vertices.push_back(aVertex);

				return static_cast<std::uint32_t>(vertexIndex);
			};

			MeshPrimitive::Triangle newTriangle;
			newTriangle.V1 = findOrAddVertex(aTriangle[0]);
			newTriangle.V2 = findOrAddVertex(aTriangle[1]);
			newTriangle.V3 = findOrAddVertex(aTriangle[2]);
			aPrimitive.Triangles.push_back(newTriangle);
		}

		void GenerateNormals_Flat(MeshPrimitive& aPrimitive)
		{
			for (const MeshPrimitive::Triangle& tri : aPrimitive.Triangles)
			{
				MeshPrimitive::Vertex& v1 = aPrimitive.Vertices[tri.V1];
				MeshPrimitive::Vertex& v2 = aPrimitive.Vertices[tri.V2];
				MeshPrimitive::Vertex& v3 = aPrimitive.Vertices[tri.V3];

				const Vector3<float> A = Vector3<float>(v1.Position.X, v1.Position.Y, v1.Position.Z);
				const Vector3<float> B = Vector3<float>(v2.Position.X, v2.Position.Y, v2.Position.Z);
				const Vector3<float> C = Vector3<float>(v3.Position.X, v3.Position.Y, v3.Position.Z);

				v1.Tangent = (B - A).Normalized();
				v1.Binormal = (C - A).Normalized();
				v1.Normal = Vector3<float>::Cross(v1.Tangent, v1.Binormal);

				v3.Tangent = v2.Tangent = v1.Tangent;
				v3.Binormal = v2.Binormal = v1.Binormal;
				v3.Normal = v2.Normal = v1.Normal;
			}
		}

		void GenerateNormals_Smooth(MeshPrimitive& aPrimitive)
		{
			for (MeshPrimitive::Vertex& vert : aPrimitive.Vertices)
			{
				vert.Normal = Vector3<float>::Zero();
				vert.Binormal = Vector3<float>::Zero();
				vert.Tangent = Vector3<float>::Zero();
			}

			for (const MeshPrimitive::Triangle& tri : aPrimitive.Triangles)
			{
				MeshPrimitive::Vertex& v1 = aPrimitive.Vertices[tri.V1];
				MeshPrimitive::Vertex& v2 = aPrimitive.Vertices[tri.V2];
				MeshPrimitive::Vertex& v3 = aPrimitive.Vertices[tri.V3];

				const Vector3<float> A = Vector3<float>(v1.Position.X, v1.Position.Y, v1.Position.Z);
				const Vector3<float> B = Vector3<float>(v2.Position.X, v2.Position.Y, v2.Position.Z);
				const Vector3<float> C = Vector3<float>(v3.Position.X, v3.Position.Y, v3.Position.Z);

				v1.Tangent = (B - A).Normalized();
				v1.Binormal = (C - A).Normalized();

				const Vector3<float> vertexNormal = Vector3<float>::Cross(v1.Tangent, v1.Binormal);
				v1.Normal += vertexNormal;
				v2.Normal += vertexNormal;
				v3.Normal += vertexNormal;

				v3.Tangent = v2.Tangent = v1.Tangent;
				v3.Binormal = v2.Binormal = v1.Binormal;
			}

			for (MeshPrimitive::Vertex& vert : aPrimitive.Vertices)
			{
				vert.Normal.Normalize();
				vert.Binormal.Normalize();
				vert.Tangent.Normalize();
			}
		}

		void Subdivide(MeshPrimitive& aPrimitive)
		{
			const std::vector<MeshPrimitive::Triangle> trianglesToSubdivide = aPrimitive.Triangles;
			aPrimitive.Triangles.clear();

			for (const MeshPrimitive::Triangle& triangle : trianglesToSubdivide)
			{
				const MeshPrimitive::Vertex v1 = aPrimitive.Vertices[triangle.V1];
				const MeshPrimitive::Vertex v2 = aPrimitive.Vertices[triangle.V2];
				const MeshPrimitive::Vertex v3 = aPrimitive.Vertices[triangle.V3];

				const MeshPrimitive::Vertex midpoint12 = LerpVertex(v1, v2, 0.5f);
				const MeshPrimitive::Vertex midpoint23 = LerpVertex(v2, v3, 0.5f);
				const MeshPrimitive::Vertex midpoint31 = LerpVertex(v3, v1, 0.5f);

				const MeshPrimitive::Vertex tri1[3] = { v1, midpoint12, midpoint31 };
				const MeshPrimitive::Vertex tri2[3] = { v2, midpoint23, midpoint12 };
				const MeshPrimitive::Vertex tri3[3] = { v3, midpoint31, midpoint23 };
				const MeshPrimitive::Vertex tri4[3] = { midpoint12, midpoint23, midpoint31 };

				AddTriangle(tri1, true, aPrimitive);
				AddTriangle(tri2, true, aPrimitive);
				AddTriangle(tri3, true, aPrimitive);
				AddTriangle(tri4, true, aPrimitive);
			}
		}

		void PopulateCapsule(MeshPrimitive& aPrimitive)
		{
			PopulateQuad(aPrimitive);
		}

		void PopulateCube(MeshPrimitive& aPrimitive)
		{
			MeshPrimitive::Vertex* v = nullptr;

			// Z-
			v = &aPrimitive.Vertices.emplace_back(); v->Position = { -1,  1, -1 }; v->UV = { 0, 1 }; v->Normal = {  0,  0, -1 }; v->Tangent = {  1,  0,  0 }; v->Binormal = {  0,  1,  0 };
			v = &aPrimitive.Vertices.emplace_back(); v->Position = {  1,  1, -1 }; v->UV = { 1, 1 }; v->Normal = {  0,  0, -1 }; v->Tangent = {  1,  0,  0 }; v->Binormal = {  0,  1,  0 };
			v = &aPrimitive.Vertices.emplace_back(); v->Position = { -1, -1, -1 }; v->UV = { 0, 0 }; v->Normal = {  0,  0, -1 }; v->Tangent = {  1,  0,  0 }; v->Binormal = {  0,  1,  0 };
			v = &aPrimitive.Vertices.emplace_back(); v->Position = { -1, -1, -1 }; v->UV = { 0, 0 }; v->Normal = {  0,  0, -1 }; v->Tangent = {  1,  0,  0 }; v->Binormal = {  0,  1,  0 };
			v = &aPrimitive.Vertices.emplace_back(); v->Position = {  1,  1, -1 }; v->UV = { 1, 1 }; v->Normal = {  0,  0, -1 }; v->Tangent = {  1,  0,  0 }; v->Binormal = {  0,  1,  0 };
			v = &aPrimitive.Vertices.emplace_back(); v->Position = {  1, -1, -1 }; v->UV = { 1, 0 }; v->Normal = {  0,  0, -1 }; v->Tangent = {  1,  0,  0 }; v->Binormal = {  0,  1,  0 };

 			//  Z+
 			v = &aPrimitive.Vertices.emplace_back(); v->Position = {  1,  1,  1 }; v->UV = { 0, 1 }; v->Normal = {  0,  0,  1 }; v->Tangent = { -1,  0,  0 }; v->Binormal = {  0,  1,  0 };
			v = &aPrimitive.Vertices.emplace_back(); v->Position = { -1,  1,  1 }; v->UV = { 1, 1 }; v->Normal = {  0,  0,  1 }; v->Tangent = { -1,  0,  0 }; v->Binormal = {  0,  1,  0 };
			v = &aPrimitive.Vertices.emplace_back(); v->Position = {  1, -1,  1 }; v->UV = { 0, 0 }; v->Normal = {  0,  0,  1 }; v->Tangent = { -1,  0,  0 }; v->Binormal = {  0,  1,  0 };
			v = &aPrimitive.Vertices.emplace_back(); v->Position = {  1, -1,  1 }; v->UV = { 0, 0 }; v->Normal = {  0,  0,  1 }; v->Tangent = { -1,  0,  0 }; v->Binormal = {  0,  1,  0 };
			v = &aPrimitive.Vertices.emplace_back(); v->Position = { -1,  1,  1 }; v->UV = { 1, 1 }; v->Normal = {  0,  0,  1 }; v->Tangent = { -1,  0,  0 }; v->Binormal = {  0,  1,  0 };
			v = &aPrimitive.Vertices.emplace_back(); v->Position = { -1, -1,  1 }; v->UV = { 1, 0 }; v->Normal = {  0,  0,  1 }; v->Tangent = { -1,  0,  0 }; v->Binormal = {  0,  1,  0 };

 			//  X+
 			v = &aPrimitive.Vertices.emplace_back(); v->Position = {  1,  1, -1 }; v->UV = { 0, 1 }; v->Normal = {  1,  0,  0 }; v->Tangent = {  0,  0,  1 }; v->Binormal = {  0,  1,  0 };
			v = &aPrimitive.Vertices.emplace_back(); v->Position = {  1,  1,  1 }; v->UV = { 1, 1 }; v->Normal = {  1,  0,  0 }; v->Tangent = {  0,  0,  1 }; v->Binormal = {  0,  1,  0 };
			v = &aPrimitive.Vertices.emplace_back(); v->Position = {  1, -1, -1 }; v->UV = { 0, 0 }; v->Normal = {  1,  0,  0 }; v->Tangent = {  0,  0,  1 }; v->Binormal = {  0,  1,  0 };
			v = &aPrimitive.Vertices.emplace_back(); v->Position = {  1, -1, -1 }; v->UV = { 0, 0 }; v->Normal = {  1,  0,  0 }; v->Tangent = {  0,  0,  1 }; v->Binormal = {  0,  1,  0 };
			v = &aPrimitive.Vertices.emplace_back(); v->Position = {  1,  1,  1 }; v->UV = { 1, 1 }; v->Normal = {  1,  0,  0 }; v->Tangent = {  0,  0,  1 }; v->Binormal = {  0,  1,  0 };
			v = &aPrimitive.Vertices.emplace_back(); v->Position = {  1, -1,  1 }; v->UV = { 1, 0 }; v->Normal = {  1,  0,  0 }; v->Tangent = {  0,  0,  1 }; v->Binormal = {  0,  1,  0 };

 			//  X-
 			v = &aPrimitive.Vertices.emplace_back(); v->Position = { -1,  1,  1 }; v->UV = { 0, 1 }; v->Normal = { -1,  0,  0 }; v->Tangent = {  0,  0, -1 }; v->Binormal = {  0,  1,  0 };
			v = &aPrimitive.Vertices.emplace_back(); v->Position = { -1,  1, -1 }; v->UV = { 1, 1 }; v->Normal = { -1,  0,  0 }; v->Tangent = {  0,  0, -1 }; v->Binormal = {  0,  1,  0 };
			v = &aPrimitive.Vertices.emplace_back(); v->Position = { -1, -1,  1 }; v->UV = { 0, 0 }; v->Normal = { -1,  0,  0 }; v->Tangent = {  0,  0, -1 }; v->Binormal = {  0,  1,  0 };
			v = &aPrimitive.Vertices.emplace_back(); v->Position = { -1, -1,  1 }; v->UV = { 0, 0 }; v->Normal = { -1,  0,  0 }; v->Tangent = {  0,  0, -1 }; v->Binormal = {  0,  1,  0 };
			v = &aPrimitive.Vertices.emplace_back(); v->Position = { -1,  1, -1 }; v->UV = { 1, 1 }; v->Normal = { -1,  0,  0 }; v->Tangent = {  0,  0, -1 }; v->Binormal = {  0,  1,  0 };
			v = &aPrimitive.Vertices.emplace_back(); v->Position = { -1, -1, -1 }; v->UV = { 1, 0 }; v->Normal = { -1,  0,  0 }; v->Tangent = {  0,  0, -1 }; v->Binormal = {  0,  1,  0 };

 			//  Y+
 			v = &aPrimitive.Vertices.emplace_back(); v->Position = { -1,  1,  1 }; v->UV = { 0, 1 }; v->Normal = {  0,  1,  0 }; v->Tangent = {  1,  0,  0 }; v->Binormal = {  0,  0,  1 };
			v = &aPrimitive.Vertices.emplace_back(); v->Position = {  1,  1,  1 }; v->UV = { 1, 1 }; v->Normal = {  0,  1,  0 }; v->Tangent = {  1,  0,  0 }; v->Binormal = {  0,  0,  1 };
			v = &aPrimitive.Vertices.emplace_back(); v->Position = { -1,  1, -1 }; v->UV = { 0, 0 }; v->Normal = {  0,  1,  0 }; v->Tangent = {  1,  0,  0 }; v->Binormal = {  0,  0,  1 };
			v = &aPrimitive.Vertices.emplace_back(); v->Position = { -1,  1, -1 }; v->UV = { 0, 0 }; v->Normal = {  0,  1,  0 }; v->Tangent = {  1,  0,  0 }; v->Binormal = {  0,  0,  1 };
			v = &aPrimitive.Vertices.emplace_back(); v->Position = {  1,  1,  1 }; v->UV = { 1, 1 }; v->Normal = {  0,  1,  0 }; v->Tangent = {  1,  0,  0 }; v->Binormal = {  0,  0,  1 };
			v = &aPrimitive.Vertices.emplace_back(); v->Position = {  1,  1, -1 }; v->UV = { 1, 0 }; v->Normal = {  0,  1,  0 }; v->Tangent = {  1,  0,  0 }; v->Binormal = {  0,  0,  1 };

 			//  Y-
 			v = &aPrimitive.Vertices.emplace_back(); v->Position = { -1, -1, -1 }; v->UV = { 0, 1 }; v->Normal = {  0, -1,  0 }; v->Tangent = {  1,  0,  0 }; v->Binormal = {  0,  0, -1 };
			v = &aPrimitive.Vertices.emplace_back(); v->Position = {  1, -1, -1 }; v->UV = { 1, 1 }; v->Normal = {  0, -1,  0 }; v->Tangent = {  1,  0,  0 }; v->Binormal = {  0,  0, -1 };
			v = &aPrimitive.Vertices.emplace_back(); v->Position = { -1, -1,  1 }; v->UV = { 0, 0 }; v->Normal = {  0, -1,  0 }; v->Tangent = {  1,  0,  0 }; v->Binormal = {  0,  0, -1 };
			v = &aPrimitive.Vertices.emplace_back(); v->Position = { -1, -1,  1 }; v->UV = { 0, 0 }; v->Normal = {  0, -1,  0 }; v->Tangent = {  1,  0,  0 }; v->Binormal = {  0,  0, -1 };
			v = &aPrimitive.Vertices.emplace_back(); v->Position = {  1, -1, -1 }; v->UV = { 1, 1 }; v->Normal = {  0, -1,  0 }; v->Tangent = {  1,  0,  0 }; v->Binormal = {  0,  0, -1 };
			v = &aPrimitive.Vertices.emplace_back(); v->Position = {  1, -1,  1 }; v->UV = { 1, 0 }; v->Normal = {  0, -1,  0 }; v->Tangent = {  1,  0,  0 }; v->Binormal = {  0,  0, -1 };

			GenerateNormals_Flat(aPrimitive);
		}

		void PopulateCylinder(MeshPrimitive& aPrimitive)
		{
			PopulateQuad(aPrimitive);
		}

		void PopulateDisc(MeshPrimitive& aPrimitive)
		{
			const std::size_t first = aPrimitive.Vertices.size();

			{
				MeshPrimitive::Vertex* v = nullptr;
				v = &aPrimitive.Vertices.emplace_back(); v->Position = { 0, 0, 0 }; v->UV = { 0, 0 };

				static constexpr std::size_t numSegments = 24;
				for (std::size_t i = 0; i < numSegments; i++)
				{
					const float theta = Atrium::TwoPi * static_cast<float>(i) / static_cast<float>(numSegments);

					const float x = cosf(theta);
					const float y = sinf(theta);
					v = &aPrimitive.Vertices.emplace_back(); v->Position = { x, 0, -y }; v->UV = { 0, 0 };
				}
			}

			for (std::size_t i = 1; i < aPrimitive.Vertices.size() - 1; ++i)
				aPrimitive.Triangles.emplace_back(MeshPrimitive::Triangle { static_cast<std::uint32_t>(first + 0), static_cast<std::uint32_t>(first + i), static_cast<std::uint32_t>(first + i + 1) });
			aPrimitive.Triangles.emplace_back(MeshPrimitive::Triangle { static_cast<std::uint32_t>(first + 0), static_cast<std::uint32_t>(first + aPrimitive.Vertices.size() - 1), static_cast<std::uint32_t>(first + 1) });

			GenerateNormals_Flat(aPrimitive);
		}

		void PopulatePlane(MeshPrimitive& aPrimitive)
		{
			MeshPrimitive::Vertex* v = nullptr;
			v = &aPrimitive.Vertices.emplace_back(); v->Position = { -1,  0,  1 }; v->UV = { 0, 1 };
			v = &aPrimitive.Vertices.emplace_back(); v->Position = {  1,  0,  1 }; v->UV = { 1, 1 };
			v = &aPrimitive.Vertices.emplace_back(); v->Position = { -1,  0, -1 }; v->UV = { 0, 0 };
			v = &aPrimitive.Vertices.emplace_back(); v->Position = {  1,  0, -1 }; v->UV = { 1, 0 };

			aPrimitive.Triangles.emplace_back(MeshPrimitive::Triangle { 0, 1, 2 });
			aPrimitive.Triangles.emplace_back(MeshPrimitive::Triangle { 2, 1, 3 });

			Subdivide(aPrimitive);

			GenerateNormals_Flat(aPrimitive);
		}

		void PopulateSphere(MeshPrimitive& aPrimitive)
		{
			PopulateQuad(aPrimitive);
		}

		void PopulateIcosphere(MeshPrimitive& aPrimitive)
		{
			constexpr float t = (1.0f + Atrium::Squareroot<float>(5.0f)) / 2.0f;

			{
				MeshPrimitive::Vertex* v = nullptr;
				v = &aPrimitive.Vertices.emplace_back(); v->Position = { -1,  t,  0 }; v->UV = { 0, 0 };
				v = &aPrimitive.Vertices.emplace_back(); v->Position = {  1,  t,  0 }; v->UV = { 0, 0 };
				v = &aPrimitive.Vertices.emplace_back(); v->Position = { -1, -t,  0 }; v->UV = { 0, 0 };
				v = &aPrimitive.Vertices.emplace_back(); v->Position = {  1, -t,  0 }; v->UV = { 0, 0 };

				v = &aPrimitive.Vertices.emplace_back(); v->Position = {  0, -1,  t }; v->UV = { 0, 0 };
				v = &aPrimitive.Vertices.emplace_back(); v->Position = {  0,  1,  t }; v->UV = { 0, 0 };
				v = &aPrimitive.Vertices.emplace_back(); v->Position = {  0, -1, -t }; v->UV = { 0, 0 };
				v = &aPrimitive.Vertices.emplace_back(); v->Position = {  0,  1, -t }; v->UV = { 0, 0 };

				v = &aPrimitive.Vertices.emplace_back(); v->Position = {  t,  0, -1 }; v->UV = { 0, 0 };
				v = &aPrimitive.Vertices.emplace_back(); v->Position = {  t,  0,  1 }; v->UV = { 0, 0 };
				v = &aPrimitive.Vertices.emplace_back(); v->Position = { -t,  0, -1 }; v->UV = { 0, 0 };
				v = &aPrimitive.Vertices.emplace_back(); v->Position = { -t,  0,  1 }; v->UV = { 0, 0 };
			}

			{
				aPrimitive.Triangles.emplace_back(MeshPrimitive::Triangle {  0, 11,  5 });
				aPrimitive.Triangles.emplace_back(MeshPrimitive::Triangle {  0,  5,  1 });
				aPrimitive.Triangles.emplace_back(MeshPrimitive::Triangle {  0,  1,  7 });
				aPrimitive.Triangles.emplace_back(MeshPrimitive::Triangle {  0,  7, 10 });
				aPrimitive.Triangles.emplace_back(MeshPrimitive::Triangle {  0, 10, 11 });

				aPrimitive.Triangles.emplace_back(MeshPrimitive::Triangle {  1,  5,  9 });
				aPrimitive.Triangles.emplace_back(MeshPrimitive::Triangle {  5, 11,  4 });
				aPrimitive.Triangles.emplace_back(MeshPrimitive::Triangle { 11, 10,  2 });
				aPrimitive.Triangles.emplace_back(MeshPrimitive::Triangle { 10,  7,  6 });
				aPrimitive.Triangles.emplace_back(MeshPrimitive::Triangle {  7,  1,  8 });

				aPrimitive.Triangles.emplace_back(MeshPrimitive::Triangle {  3,  9,  4 });
				aPrimitive.Triangles.emplace_back(MeshPrimitive::Triangle {  3,  4,  2 });
				aPrimitive.Triangles.emplace_back(MeshPrimitive::Triangle {  3,  2,  6 });
				aPrimitive.Triangles.emplace_back(MeshPrimitive::Triangle {  3,  6,  8 });
				aPrimitive.Triangles.emplace_back(MeshPrimitive::Triangle {  3,  8,  9 });

				aPrimitive.Triangles.emplace_back(MeshPrimitive::Triangle {  4,  9,  5 });
				aPrimitive.Triangles.emplace_back(MeshPrimitive::Triangle {  2,  4, 11 });
				aPrimitive.Triangles.emplace_back(MeshPrimitive::Triangle {  6,  2, 10 });
				aPrimitive.Triangles.emplace_back(MeshPrimitive::Triangle {  8,  6,  7 });
				aPrimitive.Triangles.emplace_back(MeshPrimitive::Triangle {  9,  8,  1 });
			}

			Subdivide(aPrimitive);

			for (MeshPrimitive::Vertex& v : aPrimitive.Vertices)
				v.Position.Normalize();

			GenerateNormals_Smooth(aPrimitive);
		}

		void PopulateQuad(MeshPrimitive& aPrimitive)
		{
			MeshPrimitive::Vertex* v = nullptr;
			v = &aPrimitive.Vertices.emplace_back(); v->Position = { -1,  1, 0 }; v->UV = { 0, 1 }; v->Normal = { 0, 0, -1 }; v->Tangent = { 1, 0, 0 }; v->Binormal = { 0, 1, 0 };
			v = &aPrimitive.Vertices.emplace_back(); v->Position = {  1,  1, 0 }; v->UV = { 1, 1 }; v->Normal = { 0, 0, -1 }; v->Tangent = { 1, 0, 0 }; v->Binormal = { 0, 1, 0 };
			v = &aPrimitive.Vertices.emplace_back(); v->Position = { -1, -1, 0 }; v->UV = { 0, 0 }; v->Normal = { 0, 0, -1 }; v->Tangent = { 1, 0, 0 }; v->Binormal = { 0, 1, 0 };
			v = &aPrimitive.Vertices.emplace_back(); v->Position = {  1, -1, 0 }; v->UV = { 1, 0 }; v->Normal = { 0, 0, -1 }; v->Tangent = { 1, 0, 0 }; v->Binormal = { 0, 1, 0 };

			aPrimitive.Triangles.emplace_back(MeshPrimitive::Triangle { 0, 1, 2 });
			aPrimitive.Triangles.emplace_back(MeshPrimitive::Triangle { 2, 1, 3 });
		}

	}

	MeshPrimitive MeshPrimitive::Generate(MeshPrimitiveType aType)
	{
		MeshPrimitive primitive;

		switch (aType)
		{
		case MeshPrimitiveType::Capsule:
			MakeMeshPrimitives::PopulateCapsule(primitive);
			break;
		case MeshPrimitiveType::Cube:
			MakeMeshPrimitives::PopulateCube(primitive);
			break;
		case MeshPrimitiveType::Cylinder:
			MakeMeshPrimitives::PopulateCylinder(primitive);
			break;
		case MeshPrimitiveType::Disc:
			MakeMeshPrimitives::PopulateDisc(primitive);
			break;
		case MeshPrimitiveType::Plane:
			MakeMeshPrimitives::PopulatePlane(primitive);
			break;
		case MeshPrimitiveType::Sphere:
			MakeMeshPrimitives::PopulateSphere(primitive);
			break;
		case MeshPrimitiveType::Icosphere:
			MakeMeshPrimitives::PopulateIcosphere(primitive);
			break;
		case MeshPrimitiveType::Quad:
			MakeMeshPrimitives::PopulateQuad(primitive);
			break;
		}

		return primitive;
	}
}
