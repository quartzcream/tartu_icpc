#include <iostream>
#include <vector>
#include <bitset>
#include <iomanip>
#include <assert.h>

using namespace std;

template<int M>
struct ModInt {
  int val;

  ModInt () {
    val = 0;
  }
  
  ModInt (int _x) {
    val = ((_x % M) + M) % M;
  }

  ModInt operator+ (const ModInt &other) {
    return ModInt(val + other.val);
  }

  ModInt operator- () {
    return ModInt(-val);
  }

  ModInt operator- (const ModInt &other) {
    return ModInt(val - other.val);
  }

  ModInt operator* (const ModInt &other) {
    return ModInt(val * other.val);
  }

  ModInt exp (int k) const {
    ModInt ans (1);
    for (int i = 0; i < k; i++) {
      ans = ans * (*this);
    }
    return ans;
  }

  ModInt inv () const {
    return exp(M - 2);
  }

  ModInt operator/ (const ModInt &other) {
    return (*this) * other.inv();
  }

  bool operator== (const ModInt &other) {
    return val == other.val;
  }

  bool operator!= (const ModInt &other) {
    return !((*this) == other);
  }

  bool operator> (const ModInt &other) {
    return val > other.val;
  }

  ModInt& operator+= (const ModInt &other) {
    val = ((*this) + other).val;
    return *this;
  }

  ModInt& operator-= (const ModInt &other) {
    val = ((*this) - other).val;
    return *this;
  }

  ModInt& operator*= (const ModInt &other) {
    val = ((*this) * other).val;
    return *this;
  }

  ModInt& operator/= (const ModInt &other) {
    val = ((*this) / other).val;
    return *this;
  }
};

template<int N>
ModInt<N> abs (ModInt<N> x) {
  return x;
}

template<int M>
ostream& operator<< (ostream &out, ModInt<M> n) {
  out << n.val;
  return out;
}

template<int M>
istream& operator>> (istream &in, ModInt<M> &n) {
  int val;
  in >> val;
  n.val = val;
  return in;
}

template<typename T>
ostream& operator<< (ostream &out, vector<T> v) {
  for (int i = 0; i < (int) v.size(); i++) {
    out << setw(6) << fixed << setprecision(4) << v[i] << " ";
  }
  return out;
}

bitset<10> add (bitset<10> p, bitset<10> q) {
  return p ^ q;
}

bitset<10> mult (bitset<10> v, bool k) {
  if (k) {
    return v;
  } else {
    return bitset<10> (0);
  }
}

bitset<10> normalize (bitset<10> v, int idx) {
  return v;
}

bitset<10> neg (bitset<10> v) {
  return v;
}

template<typename T>
vector<T> add (vector<T> p, vector<T> q) {
  for (int i = 0; i < (int) p.size(); i++) {
    p[i] += q[i];
  }
  return p;
}

template<typename T>
vector<T> mult (vector<T> p, T k) {
  for (int i = 0; i < (int) p.size(); i++) {
    p[i] *= k;
  }
  return p;
}

template<typename T>
vector<T> normalize (vector<T> v, int idx) {
  return mult(v, (T) 1 / v[idx]);
}

template<typename T>
vector<T> neg (vector<T> p) {
  return mult(p, (T) -1);
}

/* V is the class implementing a vector, T is the type within.
 * examples: <bitset<10>, bool>; <vector<double>, double> etc.
 * V must have an "add" operation defined */
template<typename V, typename T>
pair<vector<V>, pair<vector<int>, vector<int>>> diagonalize (vector<V> matrix, int width) {
  /* width is the number of columns we consider for diagonalizing.
   * all columns after that can be used for things after equal sign etc */
  int cur_row = 0;
  vector<int> crap_columns;
  vector<int> diag_columns;
  for (int i = 0; i < width; i++) {
    int row_id = -1;
    T best_val = 0; /* may want to replace with epsilon if working over reals */
    for (int j = cur_row; j < (int) matrix.size(); j++) {
      if (abs(matrix[j][i]) > abs(best_val)) {
        row_id = j;
        best_val = matrix[j][i];
      }
    }

    if (row_id == -1) {
      crap_columns.push_back(i);
    } else {
      diag_columns.push_back(i);
      swap(matrix[cur_row], matrix[row_id]);
      matrix[cur_row] = normalize(matrix[cur_row], i);
      for (int j = cur_row + 1; j < (int) matrix.size(); j++) {
        if (matrix[j][i] != 0) {
          matrix[j] = add(neg(normalize(matrix[j], i)), matrix[cur_row]);
        }
      }
      cur_row++;
    }
  }

  for (int i = (int) diag_columns.size() - 1; i >= 0; i--) {
    for (int j = i - 1; j >= 0; j--) {
      matrix[j] = add(matrix[j], neg(mult(matrix[i], matrix[j][diag_columns[i]])));
    }
  }
  
  return make_pair(matrix, make_pair(diag_columns, crap_columns));
}

template<typename V, typename T>
int rank (vector<V> matrix, int width) {
  return diagonalize<V, T>(matrix, width).second.size();
}

template<typename V, typename T>
vector<T> one_solution (vector<V> matrix, int width, vector<T> y) {
  /* finds one solution to the system Ax = y.
   * each row in matrix must have width at least width + 1.
   * aborts if there is no solution
   * (you can check whether solution exists using rank) */
  assert(matrix.size() == y.size());
  for (int i = 0; i < (int) matrix.size(); i++) {
    matrix[i][width] = y[i];
  }

  pair<vector<V>, pair<vector<int>, vector<int>>> prr = diagonalize<V, T>(matrix, width);
  vector<V> diag = prr.first;
  vector<int> diag_cols = prr.second.first;
  vector<T> ans (width, 0);
  for (int i = 0; i < (int) matrix.size(); i++) {
    if (i < (int) diag_cols.size()) {
      ans[diag_cols[i]] = diag[i][width];
    } else {
      assert(diag[i][width] == T(0)); /* replace with epsilon if working over reals */
    }
  }
  return ans;
}

template<typename V, typename T>
vector<vector<T>> homog_basis (vector<V> matrix, int width) {
  /* finds the basis of the nullspace of matrix */
  pair<vector<V>, pair<vector<int>, vector<int>>> prr = diagonalize<V, T>(matrix, width);
  vector<V> diag = prr.first;
  vector<int> diag_cols = prr.second.first;
  vector<int> crap_cols = prr.second.second;

  if (diag_cols.size() != matrix.size()) {
    return vector<vector<T>> (0);
  }

  vector<vector<T>> ans;
  for (int u : crap_cols) {
    vector<T> row (width, 0);
    row[u] = 1;
    for (int i = 0; i < (int) diag.size(); i++) {
      row[i] = -diag[diag_cols[i]][u];
    }
    ans.push_back(row);
  }
  return ans;
}  

int main () {  
  int height, width;
  cin >> height >> width;
  vector<bitset<10>> M (height);

  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      bool x;
      cin >> x;
      M[i][j] = x;
    }
  }
  
  auto hb = homog_basis<bitset<10>, bool> (M, width);
  for (auto row : hb) {
    cout << row << endl;
  }
}
