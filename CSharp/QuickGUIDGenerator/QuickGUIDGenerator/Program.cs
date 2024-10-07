using System.Windows.Forms;

public class Program
{
    [STAThread]
    public static void Main()
    {
        Guid guid = Guid.NewGuid();
        Console.Write(guid.ToString().ToUpper());
    }
}