using System;
using System.Collections.Generic;

namespace ConsoleApplication1
{
    class Program
    {
        int j = 100;

        static int Main(string[] args)
        {
            //Program pg = new Program();
            //pg.GetMyResult();
            //pg.MyMethod(500);
            //pg.MyMethod2(500, true, 'c', -5, 5, -10, 10, 20, -30, 30, 40.5f, 50.09);
            //Console.WriteLine("TEST");

            //Console.WriteLine();

            HashSet<int> set = new HashSet<int>();
            Dictionary<int, string> dict = new Dictionary<int, string>();

            MyTemplate<int, string, HashSet<int>, string> m1 = new MyTemplate<int, string, HashSet<int>, string>();
            m1.MyMethod<bool, Dictionary<int, string>>(50, "test", true, set, dict);

            Console.ReadLine();

            return 0;
        }

        void MyMethod(object arg)
        {
            Console.WriteLine("arg == " + arg);
        }

        void MyMethod2(int arg, bool test, char c1, sbyte c2, byte c3
            , short c4, ushort c5, uint c6, long c7, ulong c8,
            float c9, double c10)
        {
            Console.WriteLine("arg == " + arg);
        }

        int GetMyResult()
        {
            return 5;
        }
    }

    class MyTemplate<T, V, R, Result>
    {
        public MyTemplate()
        {
        }

        public Result MyMethod<MVar, MVar2>(T arg, V arg2, MVar arg3, R arg4, MVar2 arg5)
        {
            Console.WriteLine("T arg == " + arg + ", arg2 == " + arg2 + ", arg3 == " + arg3 + ", arg4 == " + arg4 + ", arg5 == " + arg5);

            return default(Result);
        }
    }
}
