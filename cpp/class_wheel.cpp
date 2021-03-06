/* visualize a class map by going around the colour wheel */
#include"misc.h"
int main(int argc, char ** argv){

  if(argc < 2) err("class_wheel [input binary file name]");

  str fn(argv[1]); // input file name
  cout << "input file name:" << fn << endl;
  str hfn(hdr_fn(fn)); // auto-detect header file name
  cout << "header file name:" << hfn << endl;
  size_t nrow, ncol, nband, np, i, j;
  hread(hfn, nrow, ncol, nband); // read header

  np = nrow * ncol;
  float r, g, b, h, s, v;
  if(nband != 1) err("this program defines results for 1-band images");

  // read data into float array
  float * dat = bread(fn, nrow, ncol, nband);

  // accumulate the data
  map<float, size_t> count;
  for0(i, np){
    if(count.count(dat[i]) < 1){
      count[dat[i]] = 0;
    }
    count[dat[i]] += 1;
  }

  priority_queue<float> pq;
  map<float, size_t>::iterator it;
  map<float, float> code_r, code_g, code_b;
  for(it = count.begin(); it != count.end(); it++){
    pq.push(it->first);
  }

  // number of codes: count.size()
  long int ci = 0;
  while(pq.size() > 0){
    float d = pq.top();
    pq.pop();

    if(d == 0.){
      code_r[d] = code_g[d] = code_b[d] = 0.;
    }
    else{
      s = v = 1.;
      h = 360. * (float)ci / (float)(count.size() - 1);
      hsv_to_rgb(&r, &g, &b, h, s, v);

      cout << "d=" << d << " ci=" << ci << " h,s,v=" << h <<"," << s << "," << v<< endl;
      code_r[d] = r;
      code_g[d] = g;
      code_b[d] = b;
      ci ++; // next colour index
    }
  }

  str ofn(fn + str("_wheel.bin"));
  str ohfn(fn + str("_wheel.hdr"));
  hwrite(ohfn, nrow, ncol, 3); // rgb file: 3 bands

  // write colour encoded output
  FILE * outf = fopen(ofn.c_str(), "wb");
  for0(i, np){
    r = code_r[dat[i]];
    fwrite(&r, sizeof(float), 1, outf);
  }
  for0(i, np){
    g = code_g[dat[i]];
    fwrite(&g, sizeof(float), 1, outf);
  }
  for0(i, np){
    b = code_b[dat[i]];
    fwrite(&b, sizeof(float), 1, outf);
  }
  fclose(outf);
  return 0;
}
