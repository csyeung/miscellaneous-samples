	#!/usr/bin/perl -w

	use strict;
	use Spreadsheet::XLSX;
	use IO::File;

	my $workbook = Spreadsheet::XLSX->new('Localization.xlsx');

	if ( !defined $workbook ) {

		#die $parser->error(), ".\n";
	}

	my (
		$second,     $minute,    $hour,
		$dayOfMonth, $month,     $yearOffset,
		$dayOfWeek,  $dayOfYear, $daylightSavings
	) = localtime();
	my $date = sprintf(
		"%02d.%02d.%4d %02d:%02d:%02d",
		$dayOfMonth, $month + 1, $yearOffset + 1900, $hour, $minute, $second
	);

	#my @xmlname = qw(localestring helpText);
	#my $i = 0;
	foreach my $worksheet ( @{ $workbook->{Worksheet} } ) 
	{
		my @filename;
		my @data;

		my %headers = ();
		my $sheetname = $worksheet->{Name};

		foreach my $row ( $worksheet->{MinRow} .. $worksheet->{MaxRow} ) {
			my $index = "";

			foreach my $col ( $worksheet->{MinCol} .. $worksheet->{MaxCol} ) {
				my $cell = $worksheet->{Cells}[$row][$col];

				if ($cell) {
					my $value = $cell->{Val};
					if ( $row == 0 ) {
						$headers{$col} = $value;
						$headers{$col} =~ s/\s+//g;

						if ( $col > 0 ) {
							my $filename_local = $value;
							push( @filename, $filename_local );
							$data[ $col - 1 ] = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
							$data[ $col - 1 ] .= "<entries>\n";
							$data[ $col - 1 ] .= "\t<category name=\"$sheetname\">\n";

							#print "Header: $value\n";
						}
					}
					else {
						if ( $value ne "" ) {
							if ( $col == 0 ) {
								$index = $value;
							}
							else {

								#replace the character with that will be processed into original;
								$value =~ s/\R/\r/g;
								$value =~ s/&lt;/\</g;
								$value =~ s/&gt;/\>/g;
								my $localData = "\t\t<entry key=\"$index\" dateModified=\"$date\" dateKeyModified=\"\" dateExported=\"$date\"><![CDATA[$value]]></entry>\n";
								$data[ $col - 1 ] .= $localData;

								#print $localData;
							}

					 #    $xml_doc_writer->startTag("Research", "id" => $value);
						}
					}
				}
			}

			if ( $row != 0 ) {

				#    $xml_doc_writer->endTag("Research");
			}
		}

		foreach my $dummy (@data) {
			$dummy .= "\t</category>\n</entries>";
		}

		my $x = 0;

		foreach my $OUTPUTFILE (@filename) {

			# Create the folder in the root folder
			my $folder = "locale";

			unless ( -d $folder ) {
				mkdir($folder) or die $!;
			}

			# Create the folder under the locale folder
			$folder .= "/" . $OUTPUTFILE . "_";

			#unless ( -d $folder ) {
			#	mkdir($folder) or die $!;
			#}

			open( MYFILE, "+> $folder$sheetname.xml" ) or die $!;

			print MYFILE $data[$x];

			close(MYFILE);

			$x++;
		}

		#print @filename;
		#print "\n";
		#print @data;
		
		#$i++;
	}

