
//!escape \section{Numerical integration with Simpson's rule}

//!begin_codebook
//!start
//computing power = how many times function integrate gets called
template<typename T>
double simps(T f, double a, double b) {
  return (f(a) + 4*f((a+b)/2) + f(b))*(b-a)/6;
}
template<typename T>
double integrate(T f, double a, double b, double computing_power){
  double m = (a+b)/2;
  double l = simps(f,a,m), r = simps(f,m,b), tot=simps(f,a,b);
  if (computing_power < 1) return tot;
  return integrate(f,a,m,computing_power/2)+integrate(f,m,b,computing_power/2);
}
//!finish
//!end_codebook

