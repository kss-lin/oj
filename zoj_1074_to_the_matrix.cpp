//zoj 1074:
// 输入:一个n*n 的矩阵A，n的取值范围为[1,100],矩阵中每个元素为取值范围为[-127,127]的整数。
// 输出:在矩阵A的所有子矩阵A0,A1,...Am中，假设每个子矩阵中对应的所有元素的和为SUM0,SUM1,...,SUMm,
// 求SUM0,SUM1,...,SUMm中的最大值。(注，子矩阵最小可以为1*1)

// 算法:
/*  任意一个子矩阵都是可以由左上角坐标(x1,y1)和右下角坐标(x2,y2)唯一确定。
    考虑左上角坐标位于每一列的子矩阵和的最大值，这样一共有n个最大值。然后
    取这n个最大值的最大值即可。
    问题就转变为，给定某一列的列值:j,定义满足矩阵的左上角在该列上的所有矩阵集合为Mj，定义Mj集合中每个矩阵
    的元素和分别为SUMM0,SUMM1....,求SUMM0,SUMM1,SUMM....中的最大值。
    
    考虑到在一维情况下，求最大连续子串和的问题，采用的是动态规划的方式,maxsum = max(maxsum+a[i],a[i]).
    类似的，我们对Mj集合中的矩阵按行做动态规划，如果Mj集合中的矩阵只有1列，那么就还原为1维的情况。
    事实上Mj集合中的矩阵有大于1列的情况，因此需要分别考虑每一列的情况。比如Mj集合中的矩阵最多有3列，
    j,j+1,j+2,势必要分别考虑矩阵右下角在第j列，第j+1列，第j+2列的情况，这里我们可以将多列的情况压缩为
    1列的情况，也就是要分别计算每一行从第j列累加到j+1列的值。为避免重复计算，引入辅助矩阵，其第i行，
    j列的值为第i行从第0列累加到第j列的值，记为accumate[i][j]。这样如果我们要求从第i行第j列累加到第k列的值，
    可以通过这个辅助矩阵简单得出:accumate[i][k] - accumate[i][j-1]
*/

#include <iostream>
using namespace std;
#define MAX(a,b)  ((a) > (b) ? (a):(b))

/* 一维最大连续字串和算法:
   输入:一维数组及长度
   输出:该数组中的最大连续字串和
*/
static inline int max_continuous_sub_str_sum(int str[],int size)
{
    int cur_max_sum = -127;
    int cur_sum = 0;
    
    for(int i = 0; i < size; i++) {
       cur_max_sum = MAX(cur_max_sum,str[i]);
       cur_sum += str[i];
       cur_max_sum = MAX(cur_max_sum,cur_sum);
       if(cur_sum < 0) {
            cur_sum = 0;
       }
    }
    return cur_max_sum;
}


/* 求矩阵M中的最大子矩阵元素和。
   输入:n*n矩阵数组，矩阵的行数，列数。
   输出:最大子矩阵元素和
*/
static int max_matrix_sub_matrix_sum(int** matrix, int row_number,int col_number)
{
    //构造辅助累加矩阵，第i行第j列的值为第i行第0列的元素累加到第i行第j列的元素的和。
    int** accumate = new int*[row_number];
    for(int i = 0; i < row_number; i++) {
        accumate[i] = new int[col_number];
        for(int j = 0; j < col_number; j++) {
            if(j > 0) 
                accumate[i][j] = accumate[i][j-1] + matrix[i][j];
            else
                accumate[i][j] = matrix[i][j];
        }
    }

    int cur_max_sum = -127;

    //预先分配空间，指的是某一指定列的累加值数组。
    //比如对于左上角在i列，右下角对第j列的情况，表示的就是从第i列累加到第j列的值，
    //共有n行。
    int *col_accumate = new int[row_number];
    
    //对于矩阵左上角在每一列的情况
    for(int j = 0; j < col_number; j++) {
        int cur_col_max_sum = -127;
       
        //右下角只可能在大于等于左上角的列上面
        for(int k = j; k < col_number;k++) {
            //先压缩为一维，算出从第j列到第k列的累加值数组。
            for(int i = 0; i < row_number; i++) {
                if(j > 0)
                    col_accumate[i] = accumate[i][k] - accumate[i][j-1];
                else
                    col_accumate[i] = accumate[i][k];
            }

            //对压缩后的一维情况做动态规划
            int div_1_sum = max_continuous_sub_str_sum(col_accumate, row_number);
            if(div_1_sum > cur_col_max_sum)
                cur_col_max_sum = div_1_sum;
        }
        if(cur_col_max_sum > cur_max_sum)
            cur_max_sum = cur_col_max_sum;
    }

    delete[] col_accumate;
    
    for(int i = 0; i < row_number; i++) {
        delete[] accumate[i];
    }
    delete[] accumate;
    
    return cur_max_sum;
}


int main(int argc,char *argv)
{
    int row_num,col_num;

    cin >> row_num;
    col_num = row_num;

    int **matrix = new int*[row_num];
    
    for(int i = 0; i < row_num;i++) {
        matrix[i] = new int[col_num];
        for(int j = 0; j < col_num; j++) {
            cin >> matrix[i][j];
        }
    }

    int sum = max_matrix_sub_matrix_sum(matrix,row_num,col_num);
    cout << sum << endl;

    return 0;
}


