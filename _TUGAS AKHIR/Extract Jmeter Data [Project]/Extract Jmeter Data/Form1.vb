Imports System.IO

Public Class Form1

    Private Function FetchData(ByVal filename As String) As ArrayList

        Dim temp As ArrayList = New ArrayList
        Dim reader As StreamReader = New StreamReader(filename)
        Dim xfetch As String


        While (reader.Peek <> -1)
            xfetch = reader.ReadLine
            temp.Add(xfetch)
        End While
        reader.Close()

        Return temp
    End Function

    Private Function WriteData(ByVal arrlist As ArrayList, ByVal output As String) As Boolean
        Dim writer As StreamWriter
        
        Try
            writer = New StreamWriter(output, False)
            For Each line As String In arrlist
                writer.WriteLine(line)
            Next
            writer.Close()
        Catch ex As Exception
            MsgBox(ex.Message)
            Return False
        End Try

        Return True
    End Function

    Private Function ProcessData(ByVal arrlist As ArrayList, ByVal index As Integer, ByVal optimize As Boolean, ByVal size As Integer) As ArrayList
        Dim temp As ArrayList = New ArrayList
        Dim counter As Integer = 0
        Dim itemp As Integer
        size = arrlist.Count / size

        If (optimize) Then
            itemp = 0

            If (index = 1 Or index = 9) Then
                For Each line As String In arrlist
                    counter += 1

                    Dim splitString() As String = Split(line, ",")

                    itemp += CInt(splitString(index))

                    If (counter >= size) Then
                        itemp = itemp / counter
                        temp.Add(itemp.ToString)
                        itemp = 0
                        counter = 0
                    End If
                Next

                Return temp
            End If

        End If

        For Each line As String In arrlist
            Dim splitString() As String = Split(line, ",")
            temp.Add(splitString(index))
        Next

        Return temp
    End Function

    Private Function processRespondTimeData(arrList As ArrayList) As ArrayList
        '-- 1 Respon
        '-- 9 Latency

        Dim arrRespondTime As ArrayList = ProcessData(arrList, 9, False, 1)
        Dim arrStatus As ArrayList = ProcessData(arrList, 3, False, 1)

        Dim temp As ArrayList = New ArrayList

        For i As Integer = 0 To arrList.Count - 1
            If (IsNumeric(arrStatus(i))) Then
                temp.Add(CInt(arrRespondTime(i)))
            Else
                temp.Add(999999)
            End If
        Next

        Return temp

    End Function

    Private Function simplifyData(arrList As ArrayList, size As Integer) As ArrayList
        Dim temp As ArrayList = New ArrayList
        Dim counter As Integer = 0
        Dim currentSize As Integer = arrList.Count / size

        Dim i_temp As Integer = 0

        For i As Integer = 0 To arrList.Count - 1
            counter += 1

            i_temp += arrList(i)

            If (counter >= currentSize) Then
                i_temp = i_temp / counter
                temp.Add(i_temp)
                counter = 0
                i_temp = 0
            End If
        Next

        Return temp
    End Function

    Private Sub Button1_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles Button1.Click
        ListBox1.Items.Clear()
        OpenFileDialog1.Title = "Please Select a File"
        OpenFileDialog1.InitialDirectory = "C:\"

        If (Directory.Exists("C:\Users\Ian\Desktop\")) Then
            OpenFileDialog1.InitialDirectory = "C:\Users\Ian\Desktop\"
        End If

        OpenFileDialog1.ShowDialog()
    End Sub

    Private Sub OpenFileDialog1_FileOk(ByVal sender As System.Object, ByVal e As System.ComponentModel.CancelEventArgs) Handles OpenFileDialog1.FileOk
        Dim temp As String = ""
        For Each line As String In OpenFileDialog1.FileNames
            ListBox1.Items.Add(line)
        Next
    End Sub

    Private Function generate_folder_name(ByVal filename As String) As String
        Dim temp() As String = Split(filename, ".")
        Dim xtemp As String = ""
        If (temp.Length >= 2) Then
            For i As Integer = 0 To temp.Length - 2
                xtemp &= temp(i)
            Next

            Return xtemp
        End If
        
        Return filename
    End Function

    Private Function process_status(ByVal arrlist As ArrayList) As ArrayList
        Dim temp As New ArrayList
        Dim xfalse, xtrue, xelse As Integer
        Dim str() As String

        xfalse = xtrue = xelse = 0
        For Each line As String In arrlist
            str = Split(line, ",")

            If (IsNumeric(str(3))) Then
                If (str(3) = "200") Then
                    xtrue += 1
                Else
                    xfalse += 1
                End If
            Else
                xelse += 1
            End If
        Next

        temp.Add(xtrue.ToString)
        temp.Add(xfalse.ToString)
        temp.Add(xelse.ToString)

        Return temp
    End Function

    Private Sub Button3_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles Button3.Click
        If ListBox1.Items.Count > 0 Then
            If r_time.Checked Then
                For i As Integer = 0 To ListBox1.Items.Count - 1
                    Dim tem As String = generate_folder_name(ListBox1.Items(i)) & " - respo"
                    If (Directory.Exists(tem) = False) Then
                        Directory.CreateDirectory(tem)
                    End If

                    Dim arrlist As ArrayList = FetchData(ListBox1.Items(i))

                    Dim tempArr As ArrayList = simplifyData(processRespondTimeData(arrlist), 20)

                    If (WriteData(tempArr, tem & "\latency_time_repo.jmt") = False) Then
                        MsgBox("Cannot Write Repo.jmt")
                    End If
                Next

                MsgBox("done")
            End If
        End If
        'If ListBox1.Items.Count > 0 Then
        '    Dim op As Boolean = False
        '    Dim size As Integer = 10

        '    If (mean_radio.Checked) Then op = True

        '    If (count_txt.Text <> vbNullString And IsNumeric(count_txt.Text)) Then size = CInt(count_txt.Text)

        '    If (size < 10) Then MsgBox("Your Size Average will be set to 10")

        '    Dim tem As String = ""

        '    For i As Integer = 0 To ListBox1.Items.Count - 1

        '        tem = generate_folder_name(ListBox1.Items(i)) & " - RAW"

        '        If (mean_radio.Checked) Then
        '            tem = generate_folder_name(ListBox1.Items(i)) & " - MEAN [" & size.ToString & "]"
        '        End If

        '        If (Directory.Exists(tem) = False) Then
        '            Directory.CreateDirectory(tem)
        '        End If

        '        Dim arrlist As ArrayList = FetchData(ListBox1.Items(i))

        '        Dim processed_respond_time As ArrayList = ProcessData(arrlist, 1, op, size) '-- Respond Time
        '        Dim processed_status_code As ArrayList = ProcessData(arrlist, 3, op, size) '-- Status Code
        '        Dim processed_status As ArrayList = ProcessData(arrlist, 7, op, size) '-- Status
        '        Dim processed_latency As ArrayList = ProcessData(arrlist, 9, op, size) '-- Latency




        '        If (WriteData(processed_latency, tem & "\latency.jmt") = False) Then
        '            MsgBox("Cannot Write latency.jmt")
        '        End If

        '        If (WriteData(processed_status, tem & "\status.jmt") = False) Then
        '            MsgBox("Cannot Write status.jmt")
        '        End If

        '        If (WriteData(processed_status_code, tem & "\status_code.jmt") = False) Then
        '            MsgBox("Cannot Write status_code.jmt")
        '        End If

        '        If (WriteData(processed_respond_time, tem & "\respond_time.jmt") = False) Then
        '            MsgBox("Cannot Write respond_time.jmt")
        '        End If

        '        If (WriteData(process_status(arrlist), tem & "\processed_status.jmt") = False) Then
        '            MsgBox("Cannot Write processed_status.jmt")
        '        End If
        '    Next

        '    MsgBox("Done")
        'End If
    End Sub

    Private Sub mean_radio_CheckedChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles mean_radio.CheckedChanged
        If (Not mean_radio.Checked) Then
            count_txt.ReadOnly = True
        Else
            count_txt.ReadOnly = False
        End If
    End Sub
End Class
