//=================================================================================//
// Copyright (c) 2023 Haolan Xu
//=================================================================================//
#include <iostream>
#include "silhouette.h"

void createSilh(Camera& camera, MeshBin& mesh) {
	glm::vec3 came_dir = camera.dir();
	std::vector<Mesh> m_silh = mesh.silh();
	std::vector<Mesh> new_m_silh;
	Mesh new_silhou;
	
	for (int i = 0; i < m_silh[0].vertices.size(); i++) {
		float dotProduct = glm::dot(m_silh[0].vertices[i].normal, came_dir);
		//std::cout << "x is " << m_silh[0].vertices[i].normal.x << "\ny is " << m_silh[0].vertices[i].normal.y << "\nz is " << m_silh[0].vertices[i].normal.z << std::endl;
		if (dotProduct == 0.0f) {
			new_silhou.vertices.push_back(m_silh[0].vertices[i]);
		}
	}
	//std::cout << new_silhou.vertices.size() << std::endl;

	if (new_silhou.vertices.size() != m_silh[0].vertices.size()) {
		new_m_silh.push_back(new_silhou);
		mesh.update_silhouette(new_m_silh);
	}
	std::cout << "x is " << came_dir.x << "\ny is " << came_dir.y << "\nz is " << came_dir.z << std::endl;
}