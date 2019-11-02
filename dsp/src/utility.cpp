#include "../include/utility.h"

using namespace util;

Buffer<float> *util::openAddressHash(Buffer<float> *buf) {
  Buffer<float> *temp = new Buffer<float>();
  temp->data = new float[buf->size * 3]{0};
  temp->size = buf->size * 3;
  // for(int i = 0; i < temp->size; i++)
  //   temp->add(i,0);
  for (int i = 0; i < buf->size; i++) {
    int temp_key = static_cast<int>(buf->get(i) * 1000000);
    int hash_i = temp_key % temp->size;
    while (temp->get(hash_i) != 0) {
      hash_i++;
      hash_i %= temp->size;
    }
    temp->add(hash_i, buf->get(i));
  }
  std::cout << "here\n";
  return temp;
}

void util::load(
    std::string fileName,
    std::vector<Buffer<float> *> &buf) {  // only works on mono files for now
  al::SearchPaths searchPaths;
  // searchPaths.addSearchPath("../../samples");
  // searchPaths.addSearchPath("..");
  searchPaths.addSearchPath(
      "/Users/jkilgore/Projects/EmissionControlPort/samples");
  // searchPaths.addSearchPath("",true);
  // searchPaths.addSearchPath("/Users/jkilgore/Applications/allo/EmissionControlPort/samples/voicePop.wav");
  // searchPaths.addSearchPath("../../samples");
  searchPaths.print();

  std::string filePath = searchPaths.find(fileName).filepath();
  gam::SoundFile soundFile;
  soundFile.path(filePath);

  if (!soundFile.openRead()) {
    std::cout << "We could not read " << fileName << "!" << std::endl;
    exit(1);
  }
  if (soundFile.channels() != 1) {
    std::cout << fileName << " is not a mono file" << std::endl;
    exit(1);
  }

  Buffer<float> *a = new Buffer<float>();
  a->size = soundFile.samples();
  a->data = new float[a->size];
  soundFile.read(a->data, a->size);

  // Not working correctly :(
  // if(soundFile.frameRate() != consts::SAMPLE_RATE) {
  //   Buffer<float>* b = new Buffer<float>();
  //   b->size = a->size/soundFile.frameRate() * consts::SAMPLE_RATE;
  //   b->data = new float[b->size];
  //   SRC_DATA *conversion = new SRC_DATA{a->data, b->data, a->size, b->size};
  //   conversion->src_ratio = soundFile.frameRate()/consts::SAMPLE_RATE;
  //   src_simple(conversion, 0, soundFile.channels());
  //   buf.push_back(b);
  //   std::cout<< "b->size: " << b->size << " a->size: " << a->size <<
  //   std::endl; delete[] a->data; delete conversion;

  // } else buf.push_back(a);

  buf.push_back(a);

  soundFile.close();
}
