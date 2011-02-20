function hideAndClearElement(name) {
    $("#edit-element-properties-0-form-builder-"+name).val("");
    $("#edit-element-properties-0-form-builder-"+name+"-wrapper").hide();
}
function showElement(name) {
    $("#edit-element-properties-0-form-builder-"+name+"-wrapper").show();
}

function showNodeCreationType(value) {
    switch(value) {
        case 'Element':
            hideAndClearElement('attribute');
            hideAndClearElement('xml');
            showElement("element");
            break;
        case 'Attribute':
            hideAndClearElement('element');
            hideAndClearElement('xml');
            showElement("attribute");
            break;
        case 'XML':
            hideAndClearElement('attribute');
            hideAndClearElement('element');
            showElement("xml");
            break;
    }
}

$(document).ready(function() {
    $('#edit-element-properties-0-form-builder-full').live('change', function() {
        if($(this).attr('checked')) {
            showElement('parent-path');
        }
        else {
            hideAndClearElement('parent-path');
        }
    });
    $('#edit-element-properties-0-form-builder-select').live('change', function() {
        showNodeCreationType($(this).val());
    });
    $("#ajaxForm").ajaxComplete(function(event, request, settings) {
        if($('#edit-element-properties-0-form-builder-full').attr('checked') == false) {
            hideAndClearElement('parent-path');
        }
        showNodeCreationType($('#edit-element-properties-0-form-builder-select').val());
    });
});