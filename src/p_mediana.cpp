#include <ilcplex/ilocplex.h>
ILOSTLBEGIN

// Array (de variaveis) de duas dimensoes para
typedef IloArray<IloBoolVarArray> IloBoolVarArray2;

int main(int argc, char **argv)
{
    // Decladando o ambiente
    IloEnv env;

    try
    {
        const char *filename = "./data/IN01.dat"; // Exemplo nao simetrico
        // const char* filename = "./data/IN02.dat"; // Exemplo simetrico

        if (argc >= 2)
            filename = argv[1];
        ifstream file(filename);
        if (!file)
        {
            cerr << "Arquivo nao encontrado: " << filename << endl;
            throw(-1);
        }

        // Dados

        // m - numero de clientes
        // n - numero de canditatos a facilidades
        // p - numero de facilidades escolhidas
        IloInt n, m, p;
        IloNumArray2 dist(env); // Matriz de distancias
        file >> n >> m >> p >> dist;

        IloModel model(env);

        // VARIAVEIS
        IloBoolVarArray2 x(env); // x
        for (int i = 0; i < m; i++)
        {
            x.add(IloBoolVarArray(env, n));
        }

        IloBoolVarArray y(env); // y
        for (int j = 0; j < n; j++)
        {
            y.add(IloBoolVar(env));
        }

        // RESTRICOES

        // Restricao 1
        for (int i = 0; i < m; i++)
        {
            IloExpr r1Expr(env);
            for (int j = 0; j < n; j++)
            {
                r1Expr += x[i][j];
            }
            model.add(r1Expr == 1);
            r1Expr.end(); // Preciso Liberar a memoria ou ela eh liberada no env.end() do final
        }

        // Restricao 2
        for (int j = 0; j < n; j++)
        {
            for (int i = 0; i < m; i++)
            {
                model.add(x[i][j] <= y[j]);
            }
        }

        // Restricao 3
        IloExpr r3Expr(env);
        for (int j = 0; j < n; j++)
        {
            r3Expr += y[j];
        }
        model.add(r3Expr == p);
        r3Expr.end();

        // FUNCAO OBJETIVO
        IloExpr objExpr(env);
        for (int i = 0; i < m; i++)
        {
            for (int j = 0; j < n; j++)
            {
                objExpr += dist[i][j] * x[i][j];
            }
        }
        model.add(IloMinimize(env, objExpr));
        objExpr.end();

        // SOLVER
        IloCplex cplex(model);
        cplex.solve();

        // Exportando o model
        cplex.exportModel("./model/p_mediana.lp");

        // SAIDA DA SOLUCAO
        env.out() << "Solution status = " << cplex.getStatus() << endl;
        env.out() << "Solution value = " << cplex.getObjValue() << endl;

        IloNumArray2 xSol(env, m);
        for (int i = 0; i < m; i++)
        {
            xSol[i] = IloNumArray(env);
            cplex.getValues(xSol[i], x[i]);
        }
        env.out() << "X Values = " << xSol << endl;

        IloNumArray ySol(env);
        cplex.getValues(ySol, y);
        env.out() << "Y Values = " << ySol << endl;
    }
    catch (IloException &e)
    {
        cerr << "Error: " << e << endl;
    }
    catch (...)
    {
        cerr << "Error: Desconhecido" << endl;
    }

    // Finalizando o ambiente
    env.end();
    return 0;
} // Fim main
