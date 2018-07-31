using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO;

namespace Mutiplay_Elevator_SimulationUI
{
    public partial class Form1 : Form
    {
        private int time1 = 0;
        public Form1()
        {
            InitializeComponent();
            label1.Visible = true;
            label2.Visible = false;
            label3.Visible = false;
            label4.Visible = false;
            label5.Visible = false;
            label6.Visible = false;
            label7.Visible = false;
            numericUpDown1.Visible = false;
            numericUpDown2.Visible = false;
            numericUpDown3.Visible = false;
            numericUpDown4.Visible = false;
            numericUpDown5.Visible = false;
            numericUpDown6.Visible = false;
            numericUpDown7.Visible = false;
            button1.Visible = false;
            timer1.Enabled = true;
        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            time1 += timer1.Interval;
            if(time1 > 500)
            {
                timer1.Enabled = false;
                label1.Visible = false;
                label2.Visible = true;
                label3.Visible = true;
                label4.Visible = true;
                label5.Visible = true;
                label6.Visible = true;
                label7.Visible = true;
                numericUpDown1.Visible = true;
                numericUpDown2.Visible = true;
                numericUpDown3.Visible = true;
                numericUpDown4.Visible = true;
                numericUpDown5.Visible = true;
                numericUpDown6.Visible = true;
                numericUpDown7.Visible = true;
                button1.Visible = true;
            }
        }

        private void button1_Click(object sender, EventArgs e)
        {
            if (File.Exists("Mutiplay Elevator Simulation.exe"))
            {
                if (File.Exists("Set.dat"))
                {
                    File.Delete("Set.dat");
                }
                FileStream set = File.Create("Set.dat");
                StringBuilder mesg = new StringBuilder();
                mesg.Append(numericUpDown1.Value);
                mesg.Append(' ');
                mesg.Append(numericUpDown2.Value);
                mesg.Append(' ');
                mesg.Append(numericUpDown3.Value);
                mesg.Append(' ');
                mesg.Append(numericUpDown4.Value);
                mesg.Append(' ');
                mesg.Append(numericUpDown5.Value);
                mesg.Append(' ');
                mesg.Append(numericUpDown6.Value);
                mesg.Append(' ');
                mesg.Append(numericUpDown7.Value);
                mesg.Append(' ');
                byte[] byteArray = System.Text.Encoding.ASCII.GetBytes(mesg.ToString());
                set.Write(byteArray, 0, byteArray.Length);
                set.Close();
                System.Diagnostics.Process.Start("Mutiplay Elevator Simulation.exe");
                Application.Exit();
            }     
            else
            {
                MessageBox.Show("电梯模拟程序不存在！");
            }          
        }

        private void numericUpDown6_ValueChanged(object sender, EventArgs e)
        {
            numericUpDown7.Minimum = numericUpDown6.Value;
            numericUpDown7.Maximum = 10000 + numericUpDown6.Value;
        }
    }
}
