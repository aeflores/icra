int nondet() { int a; return a; }
int nondet_bool() { int a; return a; }
int __cost;
int foo (int A, int B, int C, int D, int E, int F, int G, int H, int I, int J, int K, int L, int M, int N, int O, int P, int Q, int R) {
 goto loc_f0;

 loc_f0:
 {
 __cost++;
   if (nondet_bool()) {
    if (A >= 2) {
     goto loc_f10;
    }
   }
  goto end;
 }
 loc_f10:
 {
 __cost++;
   if (nondet_bool()) {
    int C_ = 0;
    if (A >= 1 + B) {
     C = C_;
     goto loc_f13;
    }
   }
   if (nondet_bool()) {
    int Y_0 = nondet();
    if (0 >= 1 + Y_0 && B >= A) {
     goto loc_f73;
    }
   }
   if (nondet_bool()) {
    if (B >= A) {
     goto loc_f73;
    }
   }
  goto end;
 }
 loc_f13:
 {
 __cost++;
   if (nondet_bool()) {
    int G_ = nondet();
    int F_ = nondet();
    int E_ = C;
    int D_ = 1 + D;
    if (A >= D && C >= 1 + G_) {
     D = D_;
     E = E_;
     F = F_;
     G = G_;
     goto loc_f13;
    }
   }
   if (nondet_bool()) {
    int G_ = nondet();
    int F_ = nondet();
    int C_ = nondet();
    int E_ = C;
    int D_ = 1 + D;
    if (A >= D && G_ >= C) {
     C = C_;
     D = D_;
     E = E_;
     F = F_;
     G = G_;
     goto loc_f13;
    }
   }
   if (nondet_bool()) {
    int R_ = 0;
    int C_ = 0;
    int B_ = 1 + B;
    if (C == 0 && D >= 1 + A) {
     B = B_;
     C = C_;
     R = R_;
     goto loc_f10;
    }
   }
   if (nondet_bool()) {
    if (D >= 1 + A && 0 >= 1 + C) {
     goto loc_f29;
    }
   }
   if (nondet_bool()) {
    if (D >= 1 + A && C >= 1) {
     goto loc_f29;
    }
   }
  goto end;
 }
 loc_f29:
 {
 __cost++;
   if (nondet_bool()) {
    int D_ = 1 + D;
    if (A >= D) {
     D = D_;
     goto loc_f29;
    }
   }
   if (nondet_bool()) {
    if (D >= 1 + A) {
     goto loc_f34;
    }
   }
  goto end;
 }
 loc_f34:
 {
 __cost++;
   if (nondet_bool()) {
    int I_ = 0;
    int H_ = 0;
    int D_ = 1 + D;
    if (A >= D) {
     D = D_;
     H = H_;
     I = I_;
     goto loc_f34;
    }
   }
   if (nondet_bool()) {
    int I_ = nondet();
    int H_ = nondet();
    int J_ = J + I_;
    int D_ = 1 + D;
    if (A >= D && 0 >= 1 + H_) {
     D = D_;
     H = H_;
     I = I_;
     J = J_;
     goto loc_f34;
    }
   }
   if (nondet_bool()) {
    int I_ = nondet();
    int H_ = nondet();
    int J_ = J + I_;
    int D_ = 1 + D;
    if (A >= D && H_ >= 1) {
     D = D_;
     H = H_;
     I = I_;
     J = J_;
     goto loc_f34;
    }
   }
   if (nondet_bool()) {
    int O_ = nondet();
    int N_ = nondet();
    int M_ = nondet();
    if (D >= 1 + A) {
     M = M_;
     N = N_;
     O = O_;
     goto loc_f53;
    }
   }
   if (nondet_bool()) {
    int Y_0 = nondet();
    int Q_ = nondet();
    int P_ = nondet();
    int O_ = -Q_;
    if (D >= 1 + A && 0 >= 1 + Y_0) {
     O = O_;
     P = P_;
     Q = Q_;
     goto loc_f53;
    }
   }
  goto end;
 }
 loc_f53:
 {
 __cost++;
   if (nondet_bool()) {
    if (A >= K) {
     goto loc_f55;
    }
   }
   if (nondet_bool()) {
    int B_ = 1 + B;
    if (K >= 1 + A) {
     B = B_;
     goto loc_f10;
    }
   }
  goto end;
 }
 loc_f55:
 {
 __cost++;
   if (nondet_bool()) {
    int J_ = nondet();
    int D_ = 1 + D;
    if (A >= D) {
     D = D_;
     J = J_;
     goto loc_f55;
    }
   }
   if (nondet_bool()) {
    int L_ = nondet();
    if (D >= 1 + A) {
     L = L_;
     goto loc_f61;
    }
   }
  goto end;
 }
 loc_f61:
 {
 __cost++;
   if (nondet_bool()) {
    int D_ = 1 + D;
    if (A >= D) {
     D = D_;
     goto loc_f61;
    }
   }
   if (nondet_bool()) {
    int K_ = 1 + K;
    if (D >= 1 + A) {
     K = K_;
     goto loc_f53;
    }
   }
  goto end;
 }
 loc_f73:
 end: __VERIFIER_print_hull(__cost);
 return 0;
}
void main() {
  foo(nondet(), nondet(), nondet(), nondet(), nondet(), nondet(), nondet(), nondet(), nondet(), nondet(), nondet(), nondet(), nondet(), nondet(), nondet(), nondet(), nondet(), nondet());
}
