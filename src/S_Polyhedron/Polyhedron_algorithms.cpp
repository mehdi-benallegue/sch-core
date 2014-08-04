#include <sch/S_Polyhedron/Polyhedron_algorithms.h>

#include <sch/File_Parsing/SimplestParsing.h>


using namespace sch;

Polyhedron_algorithms::Polyhedron_algorithms(void)
    :fastVertexes_(0x0)
    ,lastVertexes_(0x0)
{

}

Polyhedron_algorithms::Polyhedron_algorithms(const Polyhedron_algorithms &p)
    :triangles_(p.triangles_)
    ,fastVertexes_(0x0)
    ,lastVertexes_(0x0)
{
  for (unsigned i=0; i<p.vertexes_.size(); ++i)
  {
    vertexes_.push_back(p.vertexes_[i]->clone());
  }

  updateVertexNeighbors();

  updateFastArrays();
}

Polyhedron_algorithms::~Polyhedron_algorithms(void)
{
  if (fastVertexes_!=NULL)
  {
    delete[] fastVertexes_;
  }
  for (unsigned int i=0; i<vertexes_.size(); ++i)
  {
    delete vertexes_[i];
  }
}

const Polyhedron_algorithms& Polyhedron_algorithms::operator =(const Polyhedron_algorithms &p)
{
  if (this==&p)
  {
    return *this;
  }
  else
  {
    clear();
    triangles_=p.triangles_;

    for (unsigned i=0; i<p.vertexes_.size(); ++i)
    {
      vertexes_.push_back(p.vertexes_[i]->clone());
    }

    updateVertexNeighbors();

    updateFastArrays();


    return *this;
  }
}



void Polyhedron_algorithms::openFromFile(const std::string &filename)
{
  clear();

  FileParsing::SimplestParsing is;

  is.load(filename.c_str());

  int ent;
  is.jumpSeparators();
  is()>>ent;
  is.jumpSeparators();
  is()>>ent; //get the number of points
  is.find("\n");
  is.jumpSeparators();
  for (int g=0; g<ent; g++)
  {
    Scalar y[3];
    is()>>y[0];//get the coords
    is.jumpSeparators();
    is()>>y[1];//get the coords
    is.jumpSeparators();
    is()>>y[2];//get the coords
    is.jumpSeparators();

    S_PolyhedronVertex *v;
    v=new S_PolyhedronVertex();
    v->setCordinates(y[0],y[1],y[2]);
    v->setNumber(unsigned (vertexes_.size()));
    vertexes_.push_back(v);

  }

  while (is.find("normal:"))//get the normals
  {
    is.jumpSeparators();
    Scalar y[3];

    PolyhedronTriangle t;

    is()>>y[0];
    is.jumpSeparators();
    is()>>y[1];
    is.jumpSeparators();
    is()>>y[2];
    is.jumpSeparators();

    t.normal.Set(y[0],y[1],y[2]);
    t.normal.normalize();

    is.find("vertices");//get the indexes
    is.find("p");
    is()>>t.a;
    is.find("p");
    is.jumpSeparators();
    is()>>t.b;
    is.find("p");
    is.jumpSeparators();
    is()>>t.c;



    //updatingNeighbors
    vertexes_[t.a]->addNeighbor(vertexes_[t.b]);
    vertexes_[t.a]->addNeighbor(vertexes_[t.c]);

    vertexes_[t.b]->addNeighbor(vertexes_[t.a]);
    vertexes_[t.b]->addNeighbor(vertexes_[t.c]);

    vertexes_[t.c]->addNeighbor(vertexes_[t.a]);
    vertexes_[t.c]->addNeighbor(vertexes_[t.b]);

    triangles_.push_back(t);



  }

  for (unsigned int i=0; i<vertexes_.size(); i++)
  {
    vertexes_[i]->updateFastArrays();
  }

  deleteVertexesWithoutNeighbors();
}

void Polyhedron_algorithms::updateVertexNeighbors()
{
  for (unsigned i=0; i<triangles_.size(); ++i)
  {
    //updatingNeighbors
    vertexes_[triangles_[i].a]->addNeighbor(vertexes_[triangles_[i].b]);
    vertexes_[triangles_[i].a]->addNeighbor(vertexes_[triangles_[i].c]);

    vertexes_[triangles_[i].b]->addNeighbor(vertexes_[triangles_[i].a]);
    vertexes_[triangles_[i].b]->addNeighbor(vertexes_[triangles_[i].c]);

    vertexes_[triangles_[i].c]->addNeighbor(vertexes_[triangles_[i].a]);
    vertexes_[triangles_[i].c]->addNeighbor(vertexes_[triangles_[i].b]);
  }

  for (unsigned i=0; i<vertexes_.size(); ++i)
  {
    vertexes_[i]->updateFastArrays();
  }
}

void Polyhedron_algorithms::clear()
{
  for (unsigned int i=0; i<vertexes_.size(); ++i)
  {
    delete vertexes_[i];
  }
  vertexes_.clear();
  triangles_.clear();
  updateFastArrays();
}

void Polyhedron_algorithms::clearNeighbors()
{
  for (unsigned i=0; i<vertexes_.size(); ++i)
  {
    vertexes_[i]->clearNeighbors();
    vertexes_[i]->updateFastArrays();
  }
}

void Polyhedron_algorithms::updateFastArrays()
{
  if (fastVertexes_!=NULL)
  {
    delete[] fastVertexes_;
  }
  if (vertexes_.size()>0)
  {
    fastVertexes_=new S_PolyhedronVertex*[vertexes_.size()];
    for (unsigned int i=0; i<vertexes_.size(); ++i)
    {
      fastVertexes_[i]=vertexes_[i];
    }

    lastVertexes_=&(fastVertexes_[vertexes_.size()]);
  }
  else
  {
    fastVertexes_=lastVertexes_=NULL;
  }
}

Point3 Polyhedron_algorithms::naiveSupport(const Vector3&v)const
{
  S_PolyhedronVertex** current;

  current=fastVertexes_;

  Scalar supportH=(*current)->supportH(v);

  Vector3 best=(*current)->getCordinates();

  current++;

  while (current<lastVertexes_)
  {

    if ((*current)->supportH(v)>supportH)
    {
      supportH=(*current)->supportH(v);
      best=(*current)->getCordinates();
    }

    current++;
  }

  return best;
}

Point3 Polyhedron_algorithms::support(const Vector3&v,int &lastFeature)const
{
  S_PolyhedronVertex* current;
  Scalar supportH;

  if (lastFeature==-1)
  {
    current=*fastVertexes_;
  }
  else
  {
    current=fastVertexes_[lastFeature];
  }

  bool b=current->isHere(v);

  while (!b)
  {
    supportH= current->getNextVertexH();
    current = current->getNextVertex();
    b=current->isHere(v,supportH);
  }

  lastFeature=current->getNumber();


  return current->getCordinates();



}

void Polyhedron_algorithms::deleteVertexesWithoutNeighbors()
{
  int *cache=new int[vertexes_.size()];
  std::vector<S_PolyhedronVertex*> v;
  int index=0;

  for (unsigned i=0; i<vertexes_.size(); ++i)
  {
    if (vertexes_[i]->getNumNeighbors()>0)
    {
      v.push_back(vertexes_[i]);
      vertexes_[i]->setNumber(index);
      cache[i]=index++;
    }
    else
    {
      delete vertexes_[i];
      cache[i]=-1;
    }
  }

  for (unsigned i=0; i<triangles_.size(); ++i)
  {
    triangles_[i].a=cache[triangles_[i].a];
    triangles_[i].b=cache[triangles_[i].b];
    triangles_[i].c=cache[triangles_[i].c];
  }

  vertexes_=v;


  updateFastArrays();

  delete[] cache;

}
