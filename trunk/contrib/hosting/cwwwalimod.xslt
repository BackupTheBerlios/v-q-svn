<?xml version="1.0" encoding="iso-8859-2"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:template match="vqwww/mod/conf/mod/wwwali">Aliasy WWW</xsl:template>

<xsl:template match="vqwww/mod/conf/mod/desc/wwwali">
	Pozwala zarz�dza� poddomenami. Tutaj mo�esz skonfigurowa� adresy
	www.domena, ftp.domena.
</xsl:template>

<xsl:template name="wwwali_doms">
	<xsl:text>Domeny: </xsl:text>
	<xsl:for-each select="dom">
		<xsl:choose>
			<xsl:when test="not(@sel)">
				<a>
					<xsl:attribute name="href">
						<xsl:value-of select="$REQUEST_URI"/>
						<xsl:text disable-output-escaping="yes">?conf=1&amp;id=wwwali&amp;domid=</xsl:text>
						<xsl:value-of select="@id"/>
					</xsl:attribute>
				
					<xsl:value-of select="@name"/>
				</a>
			</xsl:when>
			<xsl:otherwise>
				<xsl:value-of select="@name"/>
			</xsl:otherwise>
		</xsl:choose>
		<xsl:text> </xsl:text>
	</xsl:for-each>
</xsl:template>

<!-- error messages -->
<xsl:template name="wwwali_inv">
	<xsl:for-each select="inv/rm">
		<tr class="err">
			<td colspan="2">
				<xsl:text>Nie mo�na usun��: prefix: </xsl:text>
				<xsl:value-of select="@prefix"/>
				<xsl:text>, katalog: </xsl:text>
				<xsl:value-of select="@dir"/>
			</td>
		</tr>
	</xsl:for-each>

	<xsl:for-each select="inv/dir">
		<tr class="err">
			<td colspan="2">
				<xsl:text>Nieprawid�owy katalog: prefix </xsl:text>
				<xsl:value-of select="@prefix"/>
				<xsl:text>, katalog: </xsl:text>
				<xsl:value-of select="@dir"/>
			</td>
		</tr>
	</xsl:for-each>

	<xsl:for-each select="inv/dirchars">
		<tr class="err">
			<td colspan="2">
				<xsl:text>Katalog zawiera� nieprawid�owe znaki: prefix </xsl:text>
				<xsl:value-of select="@prefix"/>
				<xsl:text>, katalog: </xsl:text>
				<xsl:value-of select="@dir"/>
			</td>
		</tr>
	</xsl:for-each>

	<xsl:for-each select="inv/prefix">
		<tr class="err">
			<td colspan="2">
				<xsl:text>Nieprawid�owy prefix: prefix </xsl:text>
				<xsl:value-of select="@prefix"/>
				<xsl:text>, katalog: </xsl:text>
				<xsl:value-of select="@dir"/>
			</td>
		</tr>
	</xsl:for-each>

	<xsl:for-each select="inv/prefixchars">
		<tr class="err">
			<td colspan="2">
				<xsl:text>Prefix zawiera� nieprawid�owe znaki: prefix </xsl:text>
				<xsl:value-of select="@prefix"/>
				<xsl:text>, katalog: </xsl:text>
				<xsl:value-of select="@dir"/>
			</td>
		</tr>
	</xsl:for-each>

	<xsl:for-each select="inv/rep">
		<tr class="err">
			<td colspan="2">
				<xsl:text>Nie mo�na zamieni�: prefix: </xsl:text>
				<xsl:value-of select="@prefix"/>
				<xsl:text>, katalog: </xsl:text>
				<xsl:value-of select="@dir"/>
			</td>
		</tr>
	</xsl:for-each>

	<xsl:for-each select="inv/add">
		<tr class="err">
			<td colspan="2">
				<xsl:text>Nie mo�na doda�: prefix: </xsl:text>
				<xsl:value-of select="@prefix"/>
				<xsl:text>, katalog: </xsl:text>
				<xsl:value-of select="@dir"/>
			</td>
		</tr>
	</xsl:for-each>
</xsl:template>

<xsl:template name="wwwali_form">
	<table align="center">
		
		<xsl:call-template name="wwwali_inv"/>
		
		<tr>
			<th>Prefix</th>
						<th>Katalog</th>
					</tr>

					<xsl:for-each select="item">
						<tr>
							<td align="right">
								<input>
									<xsl:attribute name="name">
										<xsl:text>itempre</xsl:text>
										<xsl:value-of select="position()"/>
									</xsl:attribute>
									<xsl:attribute name="value">
										<xsl:value-of select="@prefix"/>
									</xsl:attribute>
								</input>
							</td>
							<td>
								<input>
									<xsl:attribute name="name">
										<xsl:text>item</xsl:text>
										<xsl:value-of select="position()"/>
									</xsl:attribute>
									<xsl:attribute name="value">
										<xsl:value-of select="@val"/>
									</xsl:attribute>
								</input>
								<input type="hidden">
									<xsl:attribute name="name">
										<xsl:text>itemid</xsl:text>
										<xsl:value-of select="position()"/>
									</xsl:attribute>
									<xsl:attribute name="value">
										<xsl:value-of select="@id"/>
									</xsl:attribute>
								</input>
							</td>
						</tr>
					</xsl:for-each>

					<tr>
						<td colspan="2" align="center">
							<input class="button" type="submit" value="Zmie�"/>
						</td>
					</tr>
	</table>
</xsl:template>

<xsl:template match="vqwww/mod/wwwali">
	<form method="post">
		<table>
			<xsl:choose>
				<xsl:when test="getcant">
					<tr class="err"><td>Nie mo�na odczyta� aktualnych ustawie�.</td></tr>
				</xsl:when>
				<xsl:otherwise>
					<tr class="desc">
						<td>
							Uwaga: dla ka�dej z domen prefixy ustanawia si� osobno. Program nie sprawdza
							czy podany katalog istnieje. Aby usun�� prefix nale�y usun��
							jego nazw� i zapisa� zmiany.
						</td>
					</tr>
					
					<tr class="sec">
						<td>
							<xsl:call-template name="wwwali_doms"/>
						</td>
					</tr>


					<!-- input form -->
					<tr>
						<td>
							<xsl:call-template name="wwwali_form"/>
						</td>
					</tr>

					<input type="hidden" name="id" value="wwwali"/>
					<input type="hidden" name="conf" value="1"/>

					<input name="domid" type="hidden">
						<xsl:attribute name="value">
							<xsl:value-of select="domid/@id"/>
						</xsl:attribute>
					</input>
				</xsl:otherwise>
			</xsl:choose>
		</table>
	</form>
</xsl:template>

</xsl:stylesheet>
