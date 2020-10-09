using System;
using System.Diagnostics;
using System.Reflection;
using System.Text;

[assembly: System.Security.SecurityCritical]
[assembly: System.Security.AllowPartiallyTrustedCallers]

namespace Intercept.Helper
{
    public class ManagedLayer
    {
        [System.Security.SecuritySafeCritical]
        public static void Enter(object thisObject, object [] parameters)
        {
            StringBuilder sb = new StringBuilder();

            StackFrame sf = new StackFrame(1);
            sb.AppendLine("[Profiler] " + sf.GetMethod().Name + " called");

            if (thisObject == null)
            {
                sb.AppendLine("(static)");
            }
            else
            {
                DumpThisObject(sb, thisObject);
            }

            DumpParameters(sb, parameters);

            Console.WriteLine(sb.ToString());
        }

        private static void DumpParameters(StringBuilder sb, object[] parameters)
        {
            if (parameters == null)
            {
                sb.AppendFormat("[No args]");
                return;
            }

            sb.AppendFormat("[# of args: {0}]", parameters.Length);

            foreach (var item in parameters)
            {
                sb.AppendFormat("[arg: {0}]", item);
                sb.AppendLine();
            }
        }

        private static void DumpThisObject(StringBuilder sb, object thisObject)
        {
            Type type = thisObject.GetType();

            foreach (FieldInfo fieldInfo in type.GetFields(System.Reflection.BindingFlags.Public | System.Reflection.BindingFlags.NonPublic | System.Reflection.BindingFlags.Static | System.Reflection.BindingFlags.Instance))
            {
                object objValue = fieldInfo.GetValue(thisObject);
                sb.AppendFormat("[Field {0}: {1}]", fieldInfo.Name, objValue);
                sb.AppendLine();
            }
        }
    }
}
