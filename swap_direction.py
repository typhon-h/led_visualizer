"""
Quick and dirty python file to invert the direction of the key to led mapping
TODO: tidy into a proper mathematical expression
"""

l = """
  73, //A  0
  72, //A# 0
  71, //B  0
  70, //C  1
  69, //C# 1
  68, //D  1
  67, //D# 1
  67, //E  1
  66, //F  1
  65, //F# 1
  64, //G  1
  63, //G# 1
  63, //A  1
  62, //A# 1
  61, //B  1
  60, //C  2
  59, //C# 2
  59, //D  2
  58, //D# 2
  57, //E  2
  56, //F  2
  55, //F# 2
  54, //G  2
  54, //G# 2
  53, //A  2
  52, //A# 2
  51, //B  2
  50, //C  3
  50, //C# 3
  49, //D  3
  48, //D# 3
  47, //E  3
  46, //F  3
  45, //F# 3
  44, //G  3
  44, //G# 3
  43, //A  3
  42, //A# 3
  41, //B  3
  40, //C  4
  40, //C# 4
  39, //D  4
  38, //D# 4
  37, //E  4
  36, //F  4
  36, //F# 4
  35, //G  4
  34, //G# 4
  33, //A  4
  32, //A# 4
  31, //B  4
  30, //C  5
  30, //C# 5
  29, //D  5
  28, //D# 5
  27, //E  5
  26, //F  5
  26, //F# 5
  25, //G  5
  24, //G# 5
  23, //A  5
  22, //A# 5
  22, //B  5
  21, //C  6
  20, //C# 6
  19, //D  6
  18, //D# 6
  17, //E  6
  16, //F  6
  16, //F# 6
  15, //G  6
  14, //G# 6
  13, //A  6
  12, //A# 6
  12, //B  6
  11, //C  7
  10, //C# 7
  9, //D  7
  8, //D# 7
  7, //E  7
  6, //F  7
  6, //F# 7
  5, //G  7
  4, //G# 7
  3, //A  7
  2, //A# 7
  1, //B  7
  0, //C  8
  """

q = l.split("\n")
let = []
num = []
q = q[1:-1]
for line in q:
    w, e = line.split(',')
    let.append(e)
    num.insert(0, w)

for i in range(len(let)):
    print(num[i], ',', let[i])
