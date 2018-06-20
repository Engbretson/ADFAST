using System;
using System.Runtime.InteropServices;
using System.Text;
using System.Windows.Forms;
using System.Diagnostics;
using Microsoft.Win32;
using System.Drawing;

namespace HVE.AMS
{
	/// <summary>
	/// Summary description for MPA3io.
	/// </summary>
	public class MPA3io
	{
		const string DLL = "d:\\mpa3\\dmpa3.dll";
		private static bool bDemo;
		public static string ProgramDir;
		public static string ConfigDir;
		public static AcqSetting acq;
		//		public static AcqStatus Status;

		public MPA3io()
		{
			//
			//
		}
		static MPA3io()
		{
			bDemo = true;
			acq = new AcqSetting();
			//			Status = new AcqStatus();
		}

		public static void Init()
		{
			RegistryKey key = Registry.LocalMachine.OpenSubKey (MainForm.RegistryPath + @"\Demo");
			if (key == null)
			{
				MessageBox.Show("failed to open Demo registry section");
				return;
			}
			int Value = (int)key.GetValue ("MPA", 0);
			bDemo = Convert.ToBoolean (Value);
			key.Close();
			key = Registry.LocalMachine.OpenSubKey (MainForm.RegistryPath + @"\Files");
			if (key == null)
			{
				MessageBox.Show("failed to open Files registry section");
				return;
			}
			ProgramDir = (string)key.GetValue ("MPAprogram", "c:\\mpa3");
			ConfigDir = (string)key.GetValue ("MPAconfig", "c:\\mpa3\\config");
			key.Close();
			if (bDemo)
				return;
			try
			{
				Process MPA = new Process();
				MPA.StartInfo.FileName = ProgramDir +  "\\launchmp.exe";
				MPA.StartInfo.WorkingDirectory = ProgramDir;
				MPA.Start();
			}
			catch (Exception exc)
			{
				MessageBox.Show (exc.Message);
			}
		}
		// imports from DMPA3.DLL
		[DllImport (DLL, EntryPoint = "RunCmd")]
		private static extern void RunCmd(int nDisplay, string Command);
		//		private static extern void RunCmd(int nDisplay, StringBuilder Command);
		//		[DllImport (DLL, EntryPoint = "GetStatusData")]
		//		public static extern void GetStatus(ref AcqStatus Status, int nDevice);
		[DllImport (DLL, EntryPoint = "LVGetDat")]
		private static extern int LVGetDat([In,Out] uint [] Counts, int nDisplay);
		[DllImport (DLL, EntryPoint = "GetSettingData")]
		private static extern int GetSettingData(ref AcqSetting acq, int nDisplay);
		[DllImport (DLL, EntryPoint = "GetBlock")]
		private static extern void GetBlock(ref int Counts, int nStart, int nStop, int nStep, int nDisplay);

		private static void Command (string Cmd)
		{
			if (bDemo)
				return;
			//			StringBuilder Send = new StringBuilder(200);
			//			Send.Insert (0, Cmd);
			//			string Send = Cmd;

			RunCmd (0, Cmd);
		}
		public static void Start()
		{
			Command ("start");
		}
		public static void Stop()
		{
			Command ("halt");
		}
		public static void Continue()
		{
			Command ("continue");
		}
		public static long GetTotalCounts()
		{
			if (bDemo)
				return 0;
			// get the status of adc 1
			// because counts are detetcted coincidence these are the total counts
			//			MPA3io.GetStatus (ref Status, 1);
			// always use display 3 where the ROI counts are
			int nRet = GetSettingData (ref acq, 4);

			uint [] Counts = new uint[acq.nRange];
			nRet = LVGetDat(Counts, 4);
			long nTotal = 0;
			for (int n=0; n< acq.nRange; n++)
				nTotal += Counts[n];

			return nTotal;

			//			return (long)(Status.Total);
		}
		public static long GetRoiCounts()
		{
			if (bDemo)
				return 0;

			// always use display 3 where the ROI counts are
			int nRet = GetSettingData (ref acq, 3);

			uint [] Counts = new uint[acq.nRange];
			nRet = LVGetDat(Counts, 3);
			long nTotal = 0;
			for (int n=0; n< acq.nRange; n++)
				nTotal += Counts[n];

			return nTotal;
		}
		public static void PreAnalysis ()
		{
			Command ("run preanalysis.ctl");
		}
		public static void AfterAnalysis ()
		{
			Command ("run aftanalysis.ctl");
		}
		public static void PreTrace ()
		{
			Command ("run pretrace.ctl");
		}
		public static void AfterTrace ()
		{
			Command ("run afttrace.ctl");
		}
		public static void BlockErase ()
		{
			Command ("run block.ctl");
		}
		public static void LoadConfiguration(string Name)
		{
			// the configuration is in a mpa file
			string cmd = "mpaname="  + Name;
			Command (cmd);
			Command ("loadmpa");
			Command ("erasempa");
		}
		public static void SaveData (string Name)
		{
			// save an acquisition file
			string cmd = "mpaname="  + Name;
			Command (cmd);
			Command ("savempa");
		}
	}

	// info about the spectra
	[StructLayout(LayoutKind.Sequential)]
	public struct AcqSetting
	{
		public int nRange;	// the number of channels in the spectrum (AxB) for map
		public int prena;
		public int roimin;
		public int roimax;
		public int nregions;
		public int caluse;
		public int calpoints;
		public int param;
		public int offset;
		public int xdim;
		public int timesh;
		public int active;
		public double roipreset;
		public double ltpreset;
		public double timeoffs;
		public double dwelltime;

	}
}

