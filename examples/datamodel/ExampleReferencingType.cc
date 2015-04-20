// datamodel specific includes
#include "ExampleReferencingTypeObj.h"
#include "ExampleReferencingTypeData.h"
#include "ExampleReferencingTypeCollection.h"
#include <iostream>

ExampleReferencingType::ExampleReferencingType() : m_obj(new ExampleReferencingTypeObj()){
 m_obj->acquire();
};



ExampleReferencingType::ExampleReferencingType(const ExampleReferencingType& other) : m_obj(other.m_obj) {
  m_obj->acquire();
}

ExampleReferencingType& ExampleReferencingType::operator=(const ExampleReferencingType& other){
  if ( m_obj != nullptr) m_obj->release();
  m_obj = other.m_obj;
  return *this;
}

ExampleReferencingType::ExampleReferencingType(ExampleReferencingTypeObj* obj) : m_obj(obj){
  if(m_obj != nullptr)
    m_obj->acquire();
}

ExampleReferencingType ExampleReferencingType::clone() const {
  return {new ExampleReferencingTypeObj(*m_obj)};
}

ExampleReferencingType::~ExampleReferencingType(){
  if ( m_obj != nullptr) m_obj->release();
}


std::vector<ExampleCluster>::const_iterator ExampleReferencingType::Clusters_begin() const {
  auto ret_value = m_obj->m_Clusters->begin();
  std::advance(ret_value, m_obj->data.Clusters_begin);
  return ret_value;
}

std::vector<ExampleCluster>::const_iterator ExampleReferencingType::Clusters_end() const {
  auto ret_value = m_obj->m_Clusters->begin();
  std::advance(ret_value, m_obj->data.Clusters_end-1);
  return ++ret_value;
}

void ExampleReferencingType::addClusters(ExampleCluster& component) {
  m_obj->m_Clusters->push_back(component);
  m_obj->data.Clusters_end++;
}
std::vector<ExampleReferencingType>::const_iterator ExampleReferencingType::Refs_begin() const {
  auto ret_value = m_obj->m_Refs->begin();
  std::advance(ret_value, m_obj->data.Refs_begin);
  return ret_value;
}

std::vector<ExampleReferencingType>::const_iterator ExampleReferencingType::Refs_end() const {
  auto ret_value = m_obj->m_Refs->begin();
  std::advance(ret_value, m_obj->data.Refs_end-1);
  return ++ret_value;
}

void ExampleReferencingType::addRefs(ExampleReferencingType& component) {
  m_obj->m_Refs->push_back(component);
  m_obj->data.Refs_end++;
}

bool  ExampleReferencingType::isAvailable() const {
  if (m_obj != nullptr) {
    return true;
  }
  return false;
}

const albers::ObjectID ExampleReferencingType::getObjectID() const {
  return m_obj->id;
}


//bool operator< (const ExampleReferencingType& p1, const ExampleReferencingType& p2 ) {
//  if( p1.m_containerID == p2.m_containerID ) {
//    return p1.m_index < p2.m_index;
//  } else {
//    return p1.m_containerID < p2.m_containerID;
//  }
//}
