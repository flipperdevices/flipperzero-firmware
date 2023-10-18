<template>
  <div class="flex flex-col w-full mr-1 flex-1 m-auto">
    <div class="mb-2 text-lg font-bold border-b p-4">
      <slot name="header" />
    </div>
    <div class="p-4">
      <slot name="body" />
    </div>
    <div class="mt-2 text-lg font-bold border-t p-4">
      <slot name="footer" />
    </div>
  </div>
</template>
<script>
export default {};
</script>
<style lang=""></style>
