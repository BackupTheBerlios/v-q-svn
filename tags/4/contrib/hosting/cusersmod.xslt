<?xml version="1.0" encoding="iso-8859-2"?>
<xsl:stylesheet version="1.0"
	         xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:template match="vqwww/mod/conf/mod/users">Użytkownicy</xsl:template>

<xsl:template match="vqwww/mod/conf/mod/desc/users">
	Pokazuje listę kont pocztowych zarejestrowanych dla tej domeny, oraz związane
	z nimi informacje, takie jak data ważności, itd.
</xsl:template>

<!-- OPERATION LINK -->
<xsl:template name="users_opl">
	<xsl:param name="desc"/>
	<xsl:param name="act"/>
	
	<a>
		<xsl:attribute name="href">
			<xsl:value-of select="$REQUEST_URI"/>
			<xsl:text disable-output-escaping="yes">?conf=1&amp;id=users&amp;uid=</xsl:text>
			<xsl:value-of select="@id"/>
			<xsl:text disable-output-escaping="yes">&amp;act=</xsl:text>
			<xsl:value-of select="$act"/>
		</xsl:attribute>

		<xsl:value-of select="$desc"/>
	</a>
</xsl:template>

<!-- MAIN HTML -->
<xsl:template match="vqwww/mod/users">
	<form method="post">
		<table align="center">
			<xsl:choose>
				<xsl:when test="cant/get">
					<tr class="err"><td>Nie można pobrać aktualnej listy.</td></tr>
				</xsl:when>

				<!-- VERIFICATION -->
				<xsl:when test="ver/rm">
					<tr>
						<td colspan="2">
							Uwaga: odzyskanie danych usuniętego użytkownika, w tym jego poczty, nie jest
							możliwe.<br/>
							Usunąć użytkownika?
						</td>
					</tr>

					<tr>
						<td align="right">
							<a>
								<xsl:attribute name="href">
									<xsl:value-of select="$REQUEST_URI"/>
									<xsl:text disable-output-escaping="yes">?conf=1&amp;id=users</xsl:text>
								</xsl:attribute>
								Nie
							</a>
						</td>
						
						<td>
							<a>
								<xsl:attribute name="href">
									<xsl:value-of select="$REQUEST_URI"/>
									<xsl:text disable-output-escaping="yes">?conf=1&amp;id=users&amp;uid=</xsl:text>
									<xsl:value-of select="ver/rm/@id"/>
									<xsl:text disable-output-escaping="yes">&amp;act=r&amp;ver=1</xsl:text>
								</xsl:attribute>
								Tak
							</a>
						</td>
					</tr>
				</xsl:when>
				<xsl:otherwise>
					<!-- error messages -->
						<xsl:if test="cant/rm">
							<tr class="err"><td colspan="2">Nie można usunąć użytkownika</td></tr>
						</xsl:if>

						<!-- input form -->
						<tr>
							<th>Użytkownik</th>
							<th>Data utworzenia</th>
							<th>Data ważności</th>
							<th>Operacje</th>
						</tr>

						<xsl:for-each select="item">
							<tr>
								<xsl:if test="not(position() mod 2)">
									<xsl:attribute name="class">
										<xsl:text>sec</xsl:text>
									</xsl:attribute>
								</xsl:if>
							
							<td><xsl:value-of select="@user"/></td>
							<td><xsl:value-of select="@date_crt"/></td>
							<td><xsl:value-of select="@date_exp"/></td>
							<td>
								<xsl:call-template name="users_opl">
									<xsl:with-param name="desc">Usuń</xsl:with-param>
									<xsl:with-param name="act">r</xsl:with-param>
								</xsl:call-template>

								<input type="hidden">
									<xsl:attribute name="name">
										<xsl:text>itemid</xsl:text>
										<xsl:value-of select="position()"/>
									</xsl:attribute>
									<xsl:attribute name="value">
										<xsl:value-of select="@id"/>
									</xsl:attribute>
								</input>
								<input type="hidden">
									<xsl:attribute name="name">
										<xsl:text>itemuser</xsl:text>
										<xsl:value-of select="position()"/>
									</xsl:attribute>
									<xsl:attribute name="value">
										<xsl:value-of select="@user"/>
									</xsl:attribute>
								</input>
							</td>
						</tr>
					</xsl:for-each>
				</xsl:otherwise>
			</xsl:choose>
		</table>
	</form>
</xsl:template>

</xsl:stylesheet>
