var WinHttpReq = new ActiveXObject("WinHttp.WinHttpRequest.5.1");
WinHttpReq.Open("GET", "https://github.com/raysan5/raylib/releases/download/3.0.0/raylib-3.0.0-Win32-mingw.zip", /*async=*/false);
WinHttpReq.Send();

BinStream = new ActiveXObject("ADODB.Stream");
BinStream.Type = 1;
BinStream.Open();
BinStream.Write(WinHttpReq.ResponseBody);
BinStream.SaveToFile("raylib-3.0.0-Win32-mingw.zip");
