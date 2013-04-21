/*
 * Copyright (c) 1995, 2008, Oracle and/or its affiliates. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *   - Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *
 *   - Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 *   - Neither the name of Oracle or the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 * IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */ 

import java.net.*;
import java.nio.CharBuffer;
import java.util.ArrayList;
import java.util.Arrays;
import java.io.*;

public class CarSpyServer {

	public static char[] buf = new char[40000];
	public static int n, s; 
	public static int i = 0;
	public static String str;
	public static int packetSize = 1024;
	public static void main(String[] args) throws IOException {

		while(true){
			ServerSocket serverSocket = null;
			try {
				serverSocket = new ServerSocket(6102);
			} catch (IOException e) {
				System.err.println("Could not listen on port: 6102.");
				System.exit(1);
			}

			Socket clientSocket = null;
			try {
				clientSocket = serverSocket.accept();
			} catch (IOException e) {
				System.err.println("Accept failed.");
				System.exit(1);
			}

			PrintWriter out = new PrintWriter(clientSocket.getOutputStream(), true);
			BufferedReader in = new BufferedReader(
					new InputStreamReader(
							clientSocket.getInputStream()));

			System.out.println("Connection made with port 6102");

			//When connection is established the program continues running. If no data is sent across in 
			//about 1min 30s
			//The connections is closed and all input and output streams are shut down.
			try{	

				while(clientSocket.getInputStream().available() <= 13500){}
				while(in.ready()==true || s == 0) {

					n = in.read(buf, i*packetSize, packetSize);
					s = s + n;
					i++;
					
				}

			} 

			catch (IOException e)
			{
				System.err.println("No Data Inc");
				System.out.println("No Connection Attempt in TCP/IP");
			}

			//Notifies MCU Data Stream has been received. 
			out.println('*');

			//Size of sent data. 
			//int rDlength = receivedData.capacity()-receivedData.length();//receivedData.capacity()

			//Possible Array transformation for handling data. Works well for testing purposes.
			//char[] recDataArray = receivedData.array();
			//int length = buf.length;
			//Prints our all data received.

			out.println("LAST n = " + n + " s = "+ s + " i ="+ i);


			for(int j = 0; j < s; j++)
			{
				System.out.print(buf[j]);
			}

			str = new String(buf);

			if(str.contains("endstream"))
			{
				System.out.println("Endstream Found\n\n");
			}

			str = str.trim();
			System.out.println("\nTrimmed Length = " + str.length());
			//System.out.println(str+"\n");
			
			str = str.substring(0, 13500);
			System.out.println(str+"\n");

			//		for(int j = (s-n); j < s; j++)
			//		{
			//			System.out.print(""+j+ "="+buf[j]);
			//		}
			//i++;
			//if (recDataArray
			s = 0;
			i = 0;
			out.close();
			in.close();
			clientSocket.close();
			serverSocket.close();
		}
	}
}