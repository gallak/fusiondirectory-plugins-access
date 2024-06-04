<fieldset id="{$sectionId}" class="plugin-section{$sectionClasses}">
  <legend><span>{if !empty($sectionIcon)}<img src="{$sectionIcon|escape}" alt=""/>{/if}{$section|escape}</span></legend>
  <div>
  <table>
  
  {if $typeAuthentification == "CAS"}
    <tr class="
    subattribute
    {if !$attributes.fdAccessCASSettings.readable}nonreadable{/if}
    {if !$attributes.fdAccessCASSettings.writable}nonwritable{/if}
  ">
    <td title="{$attributes.fdAccessCASSettings.description|escape}">
      <label for="{$attributes.fdAccessCASSettings.htmlid}">
        {eval var=$attributes.fdAccessCASSettings.label}
      </label>
    </td>
    <td>{eval var=$attributes.fdAccessCASSettings.input}</td>
  </tr>  
    {/if}
    {if  $typeAuthentification == "SAML" }
      <tr class="
      subattribute
      {if !$attributes.fdAccessSAMLSettings.readable}nonreadable{/if}
      {if !$attributes.fdAccessSAMLSettings.writable}nonwritable{/if}
    ">
      <td title="{$attributes.fdAccessSAMLSettings.description|escape}">
        <label for="{$attributes.fdAccessSAMLSettings.htmlid}">
          {eval var=$attributes.fdAccessSAMLSettings.label}
        </label>
      </td>
      <td>{eval var=$attributes.fdAccessSAMLSettings.input}</td>
    </tr>  
      {/if}

      {if  $typeAuthentification == "OIDC" }
        <tr class="
        subattribute
        {if !$attributes.fdAccessOIDCSettings.readable}nonreadable{/if}
        {if !$attributes.fdAccessOIDCSettings.writable}nonwritable{/if}
      ">
        <td title="{$attributes.fdAccessOIDCSettings.description|escape}">
          <label for="{$attributes.fdAccessOIDCSettings.htmlid}">
            {eval var=$attributes.fdAccessOIDCSettings.label}
          </label>
        </td>
        <td>{eval var=$attributes.fdAccessOIDCSettings.input}</td>
      </tr> 
      {/if}
      {if  $typeAuthentification == "HEADER" }
          <tr class="
          subattribute
          {if !$attributes.fdAccessHEADERSettings.readable}nonreadable{/if}
          {if !$attributes.fdAccessHEADERSettings.writable}nonwritable{/if}
          ">
          <td title="{$attributes.fdAccessHEADERSettings.description|escape}">
            <label for="{$attributes.fdAccessHEADERSettings.htmlid}">
              {eval var=$attributes.fdAccessHEADERSettings.label}
            </label>
          </td>
          <td>{eval var=$attributes.fdAccessHEADERSettings.input}</td>
        </tr>

        {/if}
  </table>
  </div>
</fieldset>
